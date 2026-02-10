#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <exception>
#include <set>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_write_file(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t path_string_handle,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_BooleanPtr_t save_alpha_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr, true);

            if (*save_alpha_ptr && src.is_bgra())
            {
                cv::imwrite(path_string_handle, src);
            }

            cv::Mat bgr(src.size(), CV_8UC3);
            cv::cvtColor(src, bgr, src.is_bgra() ? cv::COLOR_BGRA2BGR : cv::COLOR_GRAY2BGR);
            
            auto success = cv::imwrite(path_string_handle, bgr);

            if(!success){
                throw std::invalid_argument("Unable to write file to \"" + std::string(path_string_handle) + "\".");
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}