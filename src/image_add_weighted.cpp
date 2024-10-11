#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_add_weighted(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t a_edvr_ref_ptr,
        LV_EDVRReferencePtr_t b_edvr_ref_ptr,
        double a_weight, double b_weight, double c_weight,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({a_edvr_ref_ptr, b_edvr_ref_ptr, dst_edvr_ref_ptr});
            
            lv_image a(a_edvr_ref_ptr);
            lv_image b(b_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(a);

            cv::addWeighted(a, a_weight, b, b_weight, c_weight, dst);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}