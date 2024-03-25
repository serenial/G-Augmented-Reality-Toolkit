#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr)
    {
        try
        {
            
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr, mask_edvr_ref_ptr});
            image::Image src(src_edvr_ref_ptr);
            image::Image dst(dst_edvr_ref_ptr);

            if (*has_mask_ptr)
            {
                if (dst.size() != src.size())
                {
                    throw std::invalid_argument("The destination image must be pre-sized to match the source image when copying with a mask.");
                }
                image::Image mask(mask_edvr_ref_ptr);
                src.copyTo(dst, mask);
            }
            else
            {
                // copy without mask
                src.copyTo(dst);
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}