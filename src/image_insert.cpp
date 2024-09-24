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

using LV_PointI32Ptr_t = LV_Ptr_t<LV_PointI32_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_insert(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_parent_edvr_ref_ptr,
        LV_EDVRReferencePtr_t src_child_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr,
        LV_PointI32Ptr_t offset_ptr)
    {
        try
        {

            throw_if_edvr_ref_pointers_not_unique({src_parent_edvr_ref_ptr, src_child_edvr_ref_ptr, dst_edvr_ref_ptr, mask_edvr_ref_ptr});
            lv_image src_parent(src_parent_edvr_ref_ptr);
            lv_image src_child(src_child_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(src_parent);

            // copy the src_parent to destination
            src_parent.copyTo(dst);

            // determine area to copy
            cv::Rect child_rect(cv::Point(offset_ptr->x, offset_ptr->y), src_child.size());
            cv::Rect parent_rect(cv::Point(0, 0), src_parent.size());

            // intersecting area
            cv::Rect copy_rect = child_rect & parent_rect;

            cv::Mat dst_rect = dst(copy_rect);
            cv::Mat src_rect = src_child(cv::Rect(cv::Point(0, 0), copy_rect.size()));

            if (*has_mask_ptr)
            {
                lv_image mask(mask_edvr_ref_ptr);
                src_rect.copyTo(dst_rect, mask);
            }
            else
            {
                // copy without mask
                src_rect.copyTo(dst_rect);
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}