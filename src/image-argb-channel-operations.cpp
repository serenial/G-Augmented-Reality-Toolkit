#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_argb_extract(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        uint8_t operation)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            // destination will be auto-sized so no need to force with ensure_sized_to_match()

            switch (operation)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                cv::extractChannel(src, dst, operation);
                break;
            default:
                // convert colours
                cv::cvtColor(src, dst, cv::COLOR_BGRA2GRAY);
                break;
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_argb_channel_set_value(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        uint8_t channel,
        uint8_t value)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);
            // https://stackoverflow.com/a/23518786/5609762
            (*src).reshape(1,src.size().area()).col(channel).setTo(cv::Scalar(value));
        
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}