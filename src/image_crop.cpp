#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

using LV_PointI32_t = struct
{
    int32_t x, y;
};

using LV_SizeU16_t = struct
{
    uint16_t width, height;
};

using LV_PointI32Ptr_t = LV_Ptr_t<LV_PointI32_t>;
using LV_SizeU16Ptr_t = LV_Ptr_t<LV_SizeU16_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_crop(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_PointI32Ptr_t origin_ptr,
        LV_SizeU16Ptr_t size_ptr)
    {
        try
        {

            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            cv::Rect crop_rect(origin_ptr->x, origin_ptr->y, size_ptr->width, size_ptr->height);

            (src(crop_rect)).copyTo(dst);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}