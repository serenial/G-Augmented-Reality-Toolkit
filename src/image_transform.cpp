#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>


#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_warp_affine(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_2x3MatrixPtr_t affine_mat_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_ImageSizePtr_t dst_size_ptr,
        LV_EnumCVInterpolationFlag_t interpolation_mode,
        LV_EnumCVBoarderType_t border_type,
        LV_U32RGBColour_t border_colour)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpAffine(
                src, dst, affine_mat_ptr->as_cv_mat(), *dst_size_ptr,
                interpolation_mode,
                border_type,
                dst.is_bgra() ? border_colour.get_bgra() : border_colour.get_blue());
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_warp_perspective(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_3x3MatrixPtr_t warp_mat_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_ImageSizePtr_t dst_size_ptr,
        LV_EnumCVInterpolationFlag_t interpolation_mode,
        LV_EnumCVBoarderType_t border_type,
        LV_U32RGBColour_t border_colour)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpPerspective(
                src, dst, warp_mat_ptr->as_cv_mat(), *dst_size_ptr,
                interpolation_mode,
                border_type,
                dst.is_bgra() ? border_colour.get_bgra() : border_colour.get_blue());
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}