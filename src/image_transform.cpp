#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_fixed_size_matrix.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_warp_affine(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_2x3MatrixPtr_t<double> affine_mat,
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

            auto M = cv_matx_from_lv_fixed_sized_matrix_ptr(affine_mat);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpAffine(
                src, dst, M, *dst_size_ptr,
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
        LV_3x3MatrixPtr_t<double> warp_mat,
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

            auto M = cv_matx_from_lv_fixed_sized_matrix_ptr(warp_mat);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpPerspective(
                src, dst, M, *dst_size_ptr,
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