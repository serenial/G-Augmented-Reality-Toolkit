#include <stdexcept>
#include <functional>
#include <opencv2\core.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit/lv-interop/lv-u32-colour.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;
using namespace image;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_create(
        LV_ErrorClusterPtr_t error_cluster_ptr, 
        LV_EDVRReferencePtr_t edvr_ref_ptr, 
        LV_ImageSizePtr_t image_size_ptr, 
        LV_BooleanPtr_t is_bgra_ptr, 
        LV_U32RGBColour_t init_value)
    {
        try
        {
            // create an image instance - this will initialize all the persistant data and EDVR structure
            Image image(edvr_ref_ptr, cv::Size(image_size_ptr->width, image_size_ptr->height), *is_bgra_ptr);
            if (image.is_bgra())
            {
                (*image) = init_value.get_bgra();
            }
            else
            {
                (*image) = init_value.get_blue();
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}

Image::Image(LV_EDVRReferencePtr_t edvr_ref_ptr, cv::Size size, bool is_abgr) : edvr_ref_ptr(edvr_ref_ptr),
                                                                                ctx(0),
                                                                                edvr_data_ptr(create_new_edvr_data_ptr()),
                                                                                data(get_metadata())
{
    data->mat = cv::Mat(size, is_abgr ? CV_8UC4 : CV_8UC1);
    data->locked = FROM_CPP; // exclusive access so can lock manually
}

Image::Image(LV_EDVRReferencePtr_t edvr_ref_ptr) : edvr_ref_ptr(edvr_ref_ptr), ctx(get_ctx()),
                                                   edvr_data_ptr(get_edvr_data_ptr()),
                                                   data(get_metadata())
{
    // lock
    lock(data, FROM_CPP);
}

Image::~Image()
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
    unlock(data);
    // release ref if originally added
    if (ctx)
    {
        EDVR_ReleaseRefWithContext(*edvr_ref_ptr, ctx);
    }
}

LV_EDVRContext_t Image::get_ctx()
{
    LV_EDVRContext_t ctx;
    auto err = EDVR_GetCurrentContext(&ctx);
    if (err)
    {
        throw std::runtime_error("Failed to obtain the application-context.");
    }
    return ctx;
}

LV_EDVRDataPtr_t Image::create_new_edvr_data_ptr()
{
    // initialize EDVR reference and get new EDVR data_ptr

    LV_EDVRDataPtr_t data_ptr = nullptr;
    auto err = EDVR_CreateReference(edvr_ref_ptr, &data_ptr);
    if (err)
    {
        throw std::runtime_error("Unable to create a data-reference to associate with the supplied EDVR Refnum.");
    }

    // initialize ImagePersistantData object
    Image::ImagePersistantData *image_meta_ptr = new Image::ImagePersistantData();

    // set the metadata_ptr of the new edvr data pointer
    data_ptr->metadata_ptr = reinterpret_cast<uintptr_t>(image_meta_ptr);

    // set the callback functions
    data_ptr->lock_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
    {
        // LabVIEW is trying to obtain the data referenced by the EDVR
        try
        {
            auto data = reinterpret_cast<Image::ImagePersistantData *>(ptr->metadata_ptr);
            lock(data, FROM_LABVIEW);
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
            auto data = reinterpret_cast<Image::ImagePersistantData *>(ptr->metadata_ptr);
            unlock(data);
        }
        catch (...)
        {
            return LV_ERR_bogusError;
        }
        return LV_ERR_noError;
    };

    data_ptr->delete_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
    {
        auto data = reinterpret_cast<Image::ImagePersistantData *>(ptr->metadata_ptr);
        // obtain the mutex
        std::unique_lock lk(data->m);
        // wait for the locked flag to be cleared
        // note - DVR Delete calls lock_callback_fn first so only check we aren't locked from LabVIEW side
        data->cv.wait(lk, [&]
                      { return data->locked != FROM_CPP; });
        // free the lock
        lk.unlock();
        // delete data
        delete data;
        ptr->metadata_ptr = reinterpret_cast<uintptr_t>(nullptr);
    };

    return data_ptr;
}

LV_EDVRDataPtr_t Image::get_edvr_data_ptr()
{
    LV_EDVRDataPtr_t data_ptr = nullptr;
    auto err = EDVR_AddRefWithContext(*edvr_ref_ptr, ctx, &data_ptr);
    if (err)
    {
        throw std::runtime_error("Unable to dereference the supplied EDVR Refnum to valid data in this application-context.");
    }
    return data_ptr;
}

Image::ImagePersistantData *Image::get_metadata()
{
    return reinterpret_cast<ImagePersistantData *>(edvr_data_ptr->metadata_ptr);
}

void Image::lock(Image::ImagePersistantData *d, Image::LockState_t state)
{
    // obtain the mutex
    std::unique_lock lk(d->m);
    // wait for the locked flag to be false
    d->cv.wait(lk, [&]
               { return d->locked == NONE; });
    // set the locked flag
    d->locked = state;
    // unlock and notify
    lk.unlock();
    d->cv.notify_one();
}

void Image::unlock(Image::ImagePersistantData *d)
{
    {
        // obtain the mutex
        std::lock_guard lk(d->m);
        // clear the locked flag
        d->locked = NONE;
    }
    // scoped-unlock and notify
    d->cv.notify_one();
}

bool Image::is_bgra()
{
    return mat().channels() == 4;
}

size_t Image::width()
{
    return mat().cols;
}

size_t Image::height()
{
    return mat().rows;
}

void Image::set_mat(cv::Mat new_mat)
{
    data->mat = new_mat;
}

cv::Mat Image::clone()
{
    return data->mat.clone();
}

void Image::copyTo(cv::_OutputArray dst)
{
    data->mat.copyTo(dst);
}

void Image::copyTo(cv::_OutputArray dst, cv::_InputArray mask)
{
    data->mat.copyTo(dst, mask);
}

cv::Mat const &Image::mat()
{
    return data->mat;
}

bool Image::is_greyscale()
{
    return !(is_bgra());
}

cv::Size Image::size()
{
    return mat().size();
}

cv::Size LV_ImageSize_t::size(){
    return cv::Size(width,height);
}

int Image::cv_type(){
    return mat().type();
}

void Image::ensure_sized_to_match(cv::Size target_size){
    if(size() != target_size){
        set_mat(cv::Mat(target_size, cv_type()));
    }
}