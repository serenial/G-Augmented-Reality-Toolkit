#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit/lv-interop/lv-enums.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_flip(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        uint8_t flip_mode)
    {
        try
        {
            
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            image::Image src(src_edvr_ref_ptr);
            image::Image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(src);

            cv::flip(src, dst, image_flip_enum_to_image_flip_code(flip_mode));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}