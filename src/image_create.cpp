#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_ImageSizePtr_t image_size_ptr,
        LV_BooleanPtr_t is_bgra_ptr,
        LV_U32RGBColour_t init_value)
    {
        try
        {
            // create an image instance - this will initialize all the persistant data and EDVR structure
            lv_image img(edvr_ref_ptr, *image_size_ptr, *is_bgra_ptr);
            if (img.is_bgra())
            {
                (*img) = init_value.get_bgra();
            }
            else
            {
                (*img) = init_value.get_blue();
            }
        }
        catch (...)
        {
           error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}