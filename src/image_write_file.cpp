#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <exception>
#include <filesystem>
#include <set>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
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
            lv_image src(src_edvr_ref_ptr);

            std::filesystem::path dst_path = static_cast<std::string_view>(path_string_handle);

            // if save_alpha then check filetype
            // avoid to lower case with std::set search
            if (*save_alpha_ptr && (std::set<std::filesystem::path>{".png", ".pnG", ".pNg", ".pNG", ".Png", ".PnG", ".PNg", ".PNG"}).count(dst_path.extension())==0)
            {
                throw std::invalid_argument("The output filetype must be .png to write the alpha channel data to file.");
            }

            if(*save_alpha_ptr && src.is_bgra()){
                cv::imwrite(dst_path.string(), src);
            }

            cv::Mat bgr(src.size(), CV_8UC3);
            cv::cvtColor(src, bgr, src.is_bgra() ? cv::COLOR_BGRA2BGR : cv::COLOR_GRAY2BGR);
            cv::imwrite(dst_path.string(), bgr);
            return LV_ERR_noError;
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}