#include <stdexcept>
#include <functional>
#include <opencv2\core.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-image.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

lv_image::lv_image(LV_EDVRReferencePtr_t edvr_ref_ptr, cv::Size size, bool is_abgr) : edvr_ref_ptr(edvr_ref_ptr),
                                                                                      ctx(0),
                                                                                      edvr_data_ptr(create_new_edvr_data_ptr()),
                                                                                      data(get_metadata())

{
    data->mat = cv::Mat(size, is_abgr ? CV_8UC4 : CV_8UC1);
    lock(data, CPP);
}

lv_image::lv_image(LV_EDVRReferencePtr_t edvr_ref_ptr) : edvr_ref_ptr(edvr_ref_ptr), ctx(get_ctx()),
                                                         edvr_data_ptr(get_edvr_data_ptr()),
                                                         data(get_metadata())
{
    lock(data, CPP);
}

lv_image::~lv_image()
{
    // ensure EDVR sub_array data is correct
    // set the n_dims
    edvr_data_ptr->n_dims = 2;
    // set the subArray data pointer to the cv::Mat's data
    edvr_data_ptr->sub_array.data_ptr = reinterpret_cast<uintptr_t *>(data->mat.data);
    // set subArray dimension_specifier to match cv::Mat's
    // strides/steps are in bytes, size is number of elements so we can directly use values
    edvr_data_ptr->sub_array.dimension_specifier[0] = {static_cast<size_t>(data->mat.rows), static_cast<ptrdiff_t>(data->mat.step[0])};
    edvr_data_ptr->sub_array.dimension_specifier[1] = {static_cast<size_t>(data->mat.cols), static_cast<ptrdiff_t>(data->mat.step[1])};

    // unlock
    unlock(data, CPP);

    // release ref if originally added
    if (ctx)
    {
        EDVR_ReleaseRefWithContext(*edvr_ref_ptr, ctx);
    }
}

LV_EDVRContext_t lv_image::get_ctx()
{
    LV_EDVRContext_t ctx;
    auto err = EDVR_GetCurrentContext(&ctx);
    if (err)
    {
        throw std::runtime_error("Failed to obtain the application-context.");
    }
    return ctx;
}

LV_EDVRDataPtr_t lv_image::create_new_edvr_data_ptr()
{
    // initialize EDVR reference and get new EDVR data_ptr

    LV_EDVRDataPtr_t data_ptr = nullptr;
    auto err = EDVR_CreateReference(edvr_ref_ptr, &data_ptr);
    if (err)
    {
        throw std::runtime_error("Unable to create a data-reference to associate with the supplied EDVR Refnum.");
    }

    // initialize  image_persistant_data object
    image_persistant_data *image_meta_ptr = new image_persistant_data();

    // set the metadata_ptr of the new edvr data pointer
    data_ptr->metadata_ptr = reinterpret_cast<uintptr_t>(image_meta_ptr);

    // set the callback functions
    data_ptr->lock_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
    {
        // LabVIEW is trying to obtain the data referenced by the EDVR
        try
        {
            auto data = reinterpret_cast<image_persistant_data *>(ptr->metadata_ptr);
            lock(data, LABVIEW);
        }
        catch (...)
        {
            return LV_ERR_bogusError;
        }

        return LV_ERR_noError;
    };

    data_ptr->unlock_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
    {
        try
        {
            auto data = reinterpret_cast<image_persistant_data *>(ptr->metadata_ptr);
            unlock(data, LABVIEW);
        }
        catch (...)
        {
            return LV_ERR_bogusError;
        }
        return LV_ERR_noError;
    };

    data_ptr->delete_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
    {
        auto data = reinterpret_cast<image_persistant_data *>(ptr->metadata_ptr);
        // obtain the mutex
        std::unique_lock lk(data->m);
        // wait for the locked flag to be cleared
        // note - DVR Delete calls lock_callback_fn first so only check we aren't locked from CPP side
        data->cv.wait(lk, [&]
                      { return data->locked == NONE || data->locked == LABVIEW;});
        // free the lock
        lk.unlock();
        // delete data
        delete data;
        ptr->metadata_ptr = reinterpret_cast<uintptr_t>(nullptr);
    };

    return data_ptr;
}

LV_EDVRDataPtr_t lv_image::get_edvr_data_ptr()
{
    LV_EDVRDataPtr_t data_ptr = nullptr;
    auto err = EDVR_AddRefWithContext(*edvr_ref_ptr, ctx, &data_ptr);
    if (err)
    {
        throw std::runtime_error("Unable to dereference the supplied EDVR Refnum to valid data in this application-context.");
    }
    return data_ptr;
}

lv_image::image_persistant_data *lv_image::get_metadata()
{
    return reinterpret_cast<image_persistant_data *>(edvr_data_ptr->metadata_ptr);
}

void lv_image::lock(image_persistant_data *d, lock_state_t transistion_to)
{
    // obtain the mutex
    std::unique_lock lk(d->m);
    // wait for the locked flag to be NONE
    // this will lead to deadlocks if CPP or CPP_MAPPED but that is probably desired behaviour
    d->cv.wait(lk, [&]
               { return d->locked == NONE; });
    d->locked = transistion_to;
    lk.unlock();
    d->cv.notify_all();
}

void lv_image::unlock(image_persistant_data *d, lock_state_t transition_from)
{
    {
        // obtain the mutex
        std::lock_guard lk(d->m);
        if (d->locked == transition_from)
        {
            d->locked = NONE;
        };
    }
    // scoped-unlock and notify
    d->cv.notify_all();
}

void lv_image::upgrade_to_mapped()
{
    {
        // obtain the mutex
        std::lock_guard lk(data->m);
        if (data->locked == CPP)
        {
            data->locked = CPP_MAPPED;
        };
    }
    // scoped-unlock and notify
    data->cv.notify_all();
}

void lv_image::downgrade_from_mapped()
{
    {
        // obtain the mutex
        std::lock_guard lk(data->m);
        if (data->locked == CPP_MAPPED)
        {
            data->locked = CPP;
        };
    }
    // scoped-unlock and notify
    data->cv.notify_all();
}

bool lv_image::is_bgra()
{
    return mat().channels() == 4;
}

size_t lv_image::width()
{
    return mat().cols;
}

size_t lv_image::height()
{
    return mat().rows;
}

void lv_image::set_mat(cv::Mat new_mat)
{
    data->mat = new_mat;
}

cv::Mat lv_image::clone()
{
    return data->mat.clone();
}

void lv_image::copyTo(cv::_OutputArray dst)
{
    data->mat.copyTo(dst);
}

void lv_image::copyTo(cv::_OutputArray dst, cv::_InputArray mask)
{
    data->mat.copyTo(dst, mask);
}

cv::Mat const &lv_image::mat()
{
    return data->mat;
}

bool lv_image::is_greyscale()
{
    return !(is_bgra());
}

cv::Size lv_image::size()
{
    return mat().size();
}

cv::Size LV_ImageSize_t::size()
{
    return cv::Size(width, height);
}

int lv_image::cv_type()
{
    return mat().type();
}

void lv_image::ensure_sized_to_match(cv::Size target_size)
{
    if (size() != target_size)
    {
        set_mat(cv::Mat(target_size, cv_type()));
    }
}