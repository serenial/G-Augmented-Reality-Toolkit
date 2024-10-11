#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_negate(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr
        )
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            (*dst) = 255 - (*src);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}