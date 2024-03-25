#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-u32-colour.hpp"
#include "g_ar_toolkit/lv-interop/lv-enums.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_rotate(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_BooleanPtr_t resize_dst_to_fit_ptr,
        double rotation_deg,
        uint8_t interpolation_mode,
        uint8_t border_type,
        LV_U32RGBColour_t border_colour)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            image::Image src(src_edvr_ref_ptr);
            image::Image dst(dst_edvr_ref_ptr);

            cv::Size required_dst_size = src.size();
            // get offset to centre of source image
            cv::Point2f src_centre(src.width() / 2.0, src.height() / 2.0);
            // get rotation matrix
            auto M = cv::getRotationMatrix2D(src_centre, rotation_deg, 1);

            if (*resize_dst_to_fit_ptr)
            {
                // get new bounding box
                auto dst_bounding_box = cv::RotatedRect(cv::Point2f(0, 0), src.size(), rotation_deg).boundingRect2f();
                // set translation to include offset from src centre to new dst centre
                M.at<double>(0, 2) += dst_bounding_box.width / 2.0 - src_centre.x;
                M.at<double>(1, 2) += dst_bounding_box.height / 2.0 - src_centre.y;

                required_dst_size = dst_bounding_box.size();
            }

            dst.ensure_sized_to_match(required_dst_size);

            cv::warpAffine(
                src, dst, M,
                dst.size(),
                interpolation_flag_enum_to_cv_interpolation_flag(interpolation_mode),
                border_type_enum_to_cv_border_type(border_type),
                dst.is_bgra() ? border_colour.get_bgra() : border_colour.get_blue());
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}