#include <string>
#include <opencv2/highgui.hpp>

#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-str.hpp"
#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_show_image_window(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr, LV_StringHandle_t window_name_str_handle, int32_t delay_ms)
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

            cv::imshow(lv_string_handle_to_string(window_name_str_handle), img_ptr->mat);
            cv::waitKey(delay_ms);

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

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_destroy_image_window(LV_ErrorClusterPtr_t error_cluster_ptr, LV_StringHandle_t window_name_str_handle)
    {
        try
        {
            cv::destroyWindow(lv_string_handle_to_string(window_name_str_handle));
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

            G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_destroy_all_image_windows(LV_ErrorClusterPtr_t error_cluster_ptr)
    {
        try
        {
            cv::destroyAllWindows();
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