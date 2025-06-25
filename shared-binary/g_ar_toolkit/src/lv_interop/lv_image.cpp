#include <stdexcept>
#include <functional>
#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

lv_image::lv_image(LV_EDVRReferencePtr_t edvr_ref_ptr, cv::Size size, bool is_abgr) : edvr_ref_ptr(edvr_ref_ptr),
                                                                                      ctx(0),
                                                                                      edvr_data_ptr(create_new_edvr_data_ptr()),
                                                                                      data(get_metadata())

{
    data->mat = cv::Mat(size, is_abgr ? CV_8UC4 : CV_8UC1);
    image_persistant_data_t::lock(data, image_persistant_data_t::lock_states::CPP);
}

lv_image::lv_image(LV_EDVRReferencePtr_t edvr_ref_ptr) : edvr_ref_ptr(edvr_ref_ptr), ctx(get_ctx()),
                                                         edvr_data_ptr(get_edvr_data_ptr()),
                                                         data(get_metadata())
{
    image_persistant_data_t::lock(data, image_persistant_data_t::lock_states::CPP);
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
    image_persistant_data_t::unlock(data, image_persistant_data_t::lock_states::CPP);

    // release ref if originally added
    if (ctx)
    {
        EDVR_ReleaseRefWithContext(*edvr_ref_ptr, ctx);
    }
}

lv_image::operator cv::_InputArray() { return data->mat; }
lv_image::operator cv::_OutputArray() { return data->mat; }
lv_image::operator cv::Mat *() { return &(data->mat); }
lv_image::operator const cv::Mat &() const { return data->mat; }
lv_image::operator cv::Mat &() { return data->mat; }

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

    // set the metadata_ptr of the new edvr data pointer
    data_ptr->metadata_ptr = reinterpret_cast<uintptr_t>(new image_persistant_data_t());

    // set the callback functions
    data_ptr->lock_callback_fn_ptr = &lv_image::on_labview_lock;
    data_ptr->unlock_callback_fn_ptr = &lv_image::on_labview_unlock;
    data_ptr->delete_callback_fn_ptr = &lv_image::on_labview_delete;

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

lv_image::image_persistant_data_t *lv_image::get_metadata()
{
    return reinterpret_cast<image_persistant_data_t *>(edvr_data_ptr->metadata_ptr);
}

void lv_image::image_persistant_data_t::lock(lv_image::image_persistant_data_t *d, lv_image::image_persistant_data_t::lock_states transition_to)
{
    // obtain the mutex
    std::unique_lock lk(d->m);
    // wait for the locked flag to be NONE
    // this will lead to deadlocks if CPP or CPP_MAPPED but that is probably desired behaviour
    d->cv.wait(lk, [&]
               { return d->locked == NONE; });
    d->locked = transition_to;
    lk.unlock();
    d->cv.notify_all();
}

void lv_image::image_persistant_data_t::unlock(lv_image::image_persistant_data_t *d, lv_image::image_persistant_data_t::lock_states transition_from)
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
        if (data->locked == image_persistant_data_t::lock_states::CPP)
        {
            data->locked = image_persistant_data_t::lock_states::CPP_MAPPED;
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
        if (data->locked == image_persistant_data_t::lock_states::CPP_MAPPED)
        {
            data->locked = image_persistant_data_t::lock_states::CPP;
        };
    }
    // scoped-unlock and notify
    data->cv.notify_all();
}

LV_MgErr_t lv_image::on_labview_lock(LV_EDVRDataPtr_t ptr)
{
    // LabVIEW is trying to obtain the data referenced by the EDVR
    try
    {
        auto data = reinterpret_cast<image_persistant_data_t *>(ptr->metadata_ptr);
        image_persistant_data_t::lock(data, image_persistant_data_t::lock_states::LABVIEW);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }

    return LV_ERR_noError;
}

LV_MgErr_t lv_image::on_labview_unlock(LV_EDVRDataPtr_t ptr)
{
    try
    {
        auto data = reinterpret_cast<image_persistant_data_t *>(ptr->metadata_ptr);
        image_persistant_data_t::unlock(data, image_persistant_data_t::lock_states::LABVIEW);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
    return LV_ERR_noError;
}

void lv_image::on_labview_delete(LV_EDVRDataPtr_t ptr)
{
    auto data = reinterpret_cast<image_persistant_data_t *>(ptr->metadata_ptr);
    // obtain the mutex
    std::unique_lock lk(data->m);
    // wait for the locked flag to be cleared
    // note - DVR Delete calls lock_callback_fn first so only check we aren't locked from CPP side
    data->cv.wait(lk, [&]
                  { return data->locked == image_persistant_data_t::lock_states::NONE || data->locked == image_persistant_data_t::lock_states::LABVIEW; });
    // free the lock
    lk.unlock();
    // delete data
    delete data;
    ptr->metadata_ptr = reinterpret_cast<uintptr_t>(nullptr);
}

bool lv_image::is_bgra() const
{
    return mat().channels() == 4;
}

size_t lv_image::width() const
{
    return mat().cols;
}

size_t lv_image::height() const
{
    return mat().rows;
}

void lv_image::set_mat(cv::Mat new_mat)
{
    data->mat = new_mat;
}

cv::Mat lv_image::clone() const
{
    return data->mat.clone();
}

void lv_image::copyTo(cv::_OutputArray dst) const
{
    data->mat.copyTo(dst);
}

void lv_image::copyTo(cv::_OutputArray dst, cv::_InputArray mask) const
{
    data->mat.copyTo(dst, mask);
}

cv::Mat const &lv_image::mat() const
{
    return data->mat;
}

bool lv_image::is_greyscale() const
{
    return !(is_bgra());
}

cv::Size lv_image::size() const
{
    return mat().size();
}

int lv_image::cv_type() const
{
    return mat().type();
}

void lv_image::ensure_sized_to_match(const cv::Size &target_size)
{
    if (size() != target_size)
    {
        set_mat(cv::Mat(target_size, cv_type()));
    }
}

void lv_image::ensure_sized_to_match(const lv_image &other)
{
    auto x = other.size();
}

cv::Mat lv_image::operator()(cv::Rect2i rect) const
{
    return data->mat(rect);
}



bool lv_image::is_empty() const {
    return data->mat.empty();
}