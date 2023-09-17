#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_create_image_edvr(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr, image::ImageSize *image_size_ptr, LV_BooleanPtr_t is_abgr)
    {
        try
        {
            LV_EDVRDataPtr_t data_ptr = nullptr;
            auto err = EDVR_CreateReference(edvr_ref_ptr, &data_ptr);

            if (err)
            {
                return err;
            }

            // create new Image with empty cv::Mat
            image::Image *img_ptr = new image::Image();
            img_ptr->mat = cv::Mat(image_size_ptr->height, image_size_ptr->width, *is_abgr ? CV_8UC4 : CV_8UC1);

            // store image pointer in edvr
            data_ptr->metadata_ptr = reinterpret_cast<uintptr_t>(img_ptr);

            // set the callback functions
            data_ptr->lock_callback_fn_ptr = &image::on_image_edvr_lock;
            data_ptr->unlock_callback_fn_ptr = &image::on_image_edvr_unlock;
            data_ptr->delete_callback_fn_ptr = &image::on_edvr_delete;

            img_ptr->set_edvr_sub_array_to_mat_data(data_ptr);

            return LV_ERR_noError;
        }
        catch (cv::Exception &e){
            return write_to_lv_error_cluster_ptr(error_cluster_ptr, e.code, __FUNCTION__, e.what());
        }
        catch (std::exception &e)
        {
            return write_to_lv_error_cluster_ptr(error_cluster_ptr, LV_ERR_bogusError, __FUNCTION__, e.what());
        }
        catch (...)
        {
            return write_undefined_err_to_lv_error_cluster_ptr(error_cluster_ptr, __FUNCTION__);
        }
    }
}

LV_MgErr_t image::Image::set_edvr_sub_array_to_mat_data(LV_EDVRDataPtr_t edvr_image_ptr)
{

    if (!edvr_image_ptr)
    {
        return LV_ERR_dvInvalidRefnum;
    }
    // set the n_dims
    edvr_image_ptr->n_dims = 2;
    // set the subArray data pointer to the cv::Mat's data
    edvr_image_ptr->sub_array.data_ptr = reinterpret_cast<uintptr_t *>(this->mat.data);
    // set subArray dimension_specifier to match cv::Mat's
    // strides/steps are in bytes, size is number of elements so we can directly use values
    edvr_image_ptr->sub_array.dimension_specifier[0] = {this->mat.rows, static_cast<ptrdiff_t>(this->mat.step[0])};
    edvr_image_ptr->sub_array.dimension_specifier[1] = {this->mat.cols, static_cast<ptrdiff_t>(this->mat.step[1])};

    return LV_ERR_noError;
}

LV_MgErr_t image::on_image_edvr_lock(LV_EDVRDataPtr_t edvr_image_ptr)
{
    try
    {
        image::Image *img_ptr = reinterpret_cast<image::Image *>(edvr_image_ptr->metadata_ptr);
        // lock mutex and leave it locked
        img_ptr->mutex.lock();
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }

    return LV_ERR_noError;
}

LV_MgErr_t image::on_image_edvr_unlock(LV_EDVRDataPtr_t edvr_image_ptr)
{
    try
    {
        // mutex should still be locked
        image::Image *image_ptr = reinterpret_cast<image::Image *>(edvr_image_ptr->metadata_ptr);
        image_ptr->mutex.unlock();
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
    return LV_ERR_noError;
}

void image::on_edvr_delete(LV_EDVRDataPtr_t edvr_image_ptr)
{
    image::Image *image_ptr = reinterpret_cast<image::Image *>(edvr_image_ptr->metadata_ptr);
    image_ptr->mutex.unlock();
    // delete the cv::Mat and
    delete image_ptr;
    image_ptr = nullptr;
}