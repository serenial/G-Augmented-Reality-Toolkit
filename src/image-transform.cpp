#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-fixed-size-matrix.hpp"
#include "g_ar_toolkit/lv-interop/lv-u32-colour.hpp"
#include "g_ar_toolkit/lv-interop/lv-enums.hpp"
#include "g_ar_toolkit/image/image.hpp"
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
        image::LV_ImageSizePtr_t dst_size_ptr,
        uint8_t interpolation_mode,
        uint8_t border_type,
        LV_U32RGBColour_t border_colour)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            image::Image src(src_edvr_ref_ptr);
            image::Image dst(dst_edvr_ref_ptr);

            auto M = cv_matx_from_lv_fixed_sized_matrix_ptr(affine_mat);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpAffine(
                src, dst, M, dst_size_ptr->size(),
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

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_warp_perspective(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_3x3MatrixPtr_t<double> warp_mat,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        image::LV_ImageSizePtr_t dst_size_ptr,
        uint8_t interpolation_mode,
        uint8_t border_type,
        LV_U32RGBColour_t border_colour)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            image::Image src(src_edvr_ref_ptr);
            image::Image dst(dst_edvr_ref_ptr);

            auto M = cv_matx_from_lv_fixed_sized_matrix_ptr(warp_mat);

            dst.ensure_sized_to_match(*dst_size_ptr);

            cv::warpPerspective(
                src, dst, M, dst_size_ptr->size(),
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