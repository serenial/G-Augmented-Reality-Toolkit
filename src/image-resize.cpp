#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_resize_image(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr, image::ImageSize *new_image_size_ptr, LV_BooleanPtr_t retain_data_ptr, cv::InterpolationFlags interpolation_mode)
    {
        try
        {
            LV_EDVRDataPtr_t edvr_image_ptr = nullptr;
            LV_EDVRContext_t ctx;

            auto err = get_edvr_data_handle_with_context(*edvr_ref_ptr, &ctx, &edvr_image_ptr);

            if (err)
            {
                return err;
            }

            auto img_ptr = reinterpret_cast<image::Image *>(edvr_image_ptr->metadata_ptr);
            // lock the mutex
            img_ptr->mutex.lock();

            // check if already correctly sized and short-circuit
            if (img_ptr->mat.rows == new_image_size_ptr->height && img_ptr->mat.cols == new_image_size_ptr->width)
            {
                goto done;
            }

            if (*retain_data_ptr && img_ptr->mat.rows && img_ptr->mat.cols)
            {
                // resize
                cv::Mat dst;
                cv::resize(img_ptr->mat, dst, cv::Size{new_image_size_ptr->width, new_image_size_ptr->height}, 0, 0, interpolation_mode);
                img_ptr->mat = dst;
            }
            else
            {
                // just replace the mat with a correctly sized one
                img_ptr->mat = cv::Mat(new_image_size_ptr->height, new_image_size_ptr->width, img_ptr->mat.type());
            }

            img_ptr->set_edvr_sub_array_to_mat_data(edvr_image_ptr);

        done:
            // release mutex and EDVR reference count
            img_ptr->mutex.unlock();
            return EDVR_ReleaseRefWithContext(*edvr_ref_ptr, ctx);
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