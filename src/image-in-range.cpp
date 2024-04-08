#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-u32-colour.hpp"
#include "g_ar_toolkit/lv-interop/lv-image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_in_range(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_U32RGBColour_t lower_colour,
        LV_U32RGBColour_t upper_colour,
        LV_BooleanPtr_t ignore_argb_alpha_ptr)
    {
        try
        {
            
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);
            auto ignore_alpha = *ignore_argb_alpha_ptr;

            // destination will be auto-sized so no need to force with ensure_sized_to_match()

            if (ignore_alpha)
            {
                lower_colour.alpha = 255;
                upper_colour.alpha = 255;
            }

            if (src.is_bgra())
            {
                if (lower_colour != upper_colour)
                {
                    // we need to in-range using the hsv colour space so the values work in a human-friendly way
                    cv::Mat hsv;
                    cv::Scalar lower_hsv, upper_hsv;
                    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
                    cv::inRange(hsv, lower_colour.get_hsv(), upper_colour.get_hsv(), dst);
                }
                else
                {
                    auto colour = lower_colour.get_bgr();
                    if (ignore_alpha)
                    {
                        cv::Mat bgr(src.size(), CV_8UC3);
                        cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR);
                        cv::inRange(bgr, colour, colour, dst);
                    }
                    else
                    {
                        cv::inRange(src, colour, colour, dst);
                    }
                }
            }
            else
            {
                cv::inRange(src, lower_colour.get_blue(), upper_colour.get_blue(), dst);
            }
            return LV_ERR_noError;
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}