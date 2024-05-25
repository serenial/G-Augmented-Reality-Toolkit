#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_bitwise(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t a_edvr_ref_ptr,
        LV_EDVRReferencePtr_t b_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr,
        uint8_t operation)
    {
        try
        {
            
            throw_if_edvr_ref_pointers_not_unique({a_edvr_ref_ptr, b_edvr_ref_ptr, dst_edvr_ref_ptr, mask_edvr_ref_ptr});
            lv_image a(a_edvr_ref_ptr);
            lv_image b(b_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            if(a.size() != b.size()){
                throw std::invalid_argument("The source image must have the same size.");
            }

            if (*has_mask_ptr)
            {
                if (dst.size() != a.size())
                {
                    throw std::invalid_argument("The destination image must be pre-sized to match the source images when applying a bitwise operation with a mask.");
                }
                lv_image mask(mask_edvr_ref_ptr);
                switch (operation)
                {
                case 0:
                    cv::bitwise_or(a, b, dst, mask);
                    break;
                case 1:
                    cv::bitwise_and(a, b, dst, mask);
                    break;
                case 2:
                    cv::bitwise_xor(a, b, dst, mask);
                    break;
                }
            }
            else
            {
                dst.ensure_sized_to_match(a);
                switch (operation)
                {
                case 0:
                    cv::bitwise_or(a, b, dst);
                    break;
                case 1:
                    cv::bitwise_and(a, b, dst);
                    break;
                case 2:
                    cv::bitwise_xor(a, b, dst);
                    break;
                }
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_bitwise_not(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr)
    {
        try
        {
            
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            if (*has_mask_ptr)
            {
                if (dst.size() != src.size())
                {
                    throw std::invalid_argument("The destination image must be pre-sized to match the source images when applying a bitwise operation with a mask.");
                }
                lv_image mask(mask_edvr_ref_ptr);
                cv::bitwise_not(src, dst, mask);
            }
            else
            {
                dst.ensure_sized_to_match(src);
                cv::bitwise_not(src, dst);
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}