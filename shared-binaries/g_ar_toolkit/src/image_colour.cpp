#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;
extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_compute_colour_pair(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        float fractional_change_lower,
        float fractional_change_upper,
        LV_U32RGBColour_t *lower,
        LV_U32RGBColour_t *upper)
    {
        try
        {
            lower->adjust(-fractional_change_lower);
            upper->adjust(fractional_change_upper);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}