#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_crop(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_ImagePointIntPtr_t origin_ptr,
        LV_ImageSizePtr_t size_ptr,
        LV_ImagePointIntPtr_t bottom_right_ptr,
        LV_BooleanPtr_t use_size)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            auto max_x = src.width();
            auto max_y = src.height();

            cv::Point2i bottom_right{*bottom_right_ptr};

            if (*use_size)
            {
                bottom_right = cv::Rect(*origin_ptr, *size_ptr).br();
            }

            if (bottom_right.x > max_x)
            {
                bottom_right.x = max_x;
            }
            if (bottom_right.y > max_y)
            {
                bottom_right.y = max_y;
            }

            cv::Rect crop_rect(*origin_ptr, bottom_right);

            (src(crop_rect)).copyTo(dst);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}