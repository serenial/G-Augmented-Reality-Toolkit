#include <string>
#include <opencv2/highgui.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"

#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_window_show(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr, LV_StringHandle_t window_name_str_handle)
    {
        try
        {
            cv::imshow(window_name_str_handle, lv_image(edvr_ref_ptr));
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_window_destroy(LV_ErrorClusterPtr_t error_cluster_ptr, LV_StringHandle_t window_name_str_handle)
    {
        try
        {
            cv::destroyWindow(window_name_str_handle);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
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
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}