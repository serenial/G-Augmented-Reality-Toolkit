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
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_window_show(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr, LV_StringHandle_t window_name_str_handle)
    {
        try
        {
            cv::imshow(lv_string_handle_to_string(window_name_str_handle), image::Image(edvr_ref_ptr));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_window_destroy(LV_ErrorClusterPtr_t error_cluster_ptr, LV_StringHandle_t window_name_str_handle)
    {
        try
        {
            cv::destroyWindow(lv_string_handle_to_string(window_name_str_handle));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_windows_destroy_all(LV_ErrorClusterPtr_t error_cluster_ptr)
    {
        try
        {
            cv::destroyAllWindows();
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}