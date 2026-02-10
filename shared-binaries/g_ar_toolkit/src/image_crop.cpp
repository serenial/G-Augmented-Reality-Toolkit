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
        LV_ImagePointIntPtr_t offset_ptr,
        LV_ImageSizePtr_t size_ptr,
        LV_ImagePointIntPtr_t bottom_right_ptr,
        LV_BooleanPtr_t use_size)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            offset_ptr->m_x = std::max(offset_ptr->m_x, 0);
            offset_ptr->m_y = std::max(offset_ptr->m_y, 0);

            bool has_region_size = size_ptr->m_height > 0 && size_ptr->m_width > 0;

            // create from corner points;
            cv::Rect2i crop(*offset_ptr, *bottom_right_ptr);

            if (*use_size)
            {
                if (has_region_size)
                {
                    // use specified size
                    crop = cv::Rect2i{offset_ptr->m_x, offset_ptr->m_y, size_ptr->m_width, size_ptr->m_height};
                }
                else
                {
                    // use rest of image
                    crop = cv::Rect2i{offset_ptr->m_x, offset_ptr->m_y, src.mat().cols - offset_ptr->m_x, src.mat().rows - offset_ptr->m_y};
                }
            }

            // trim crop-region to intersection of crop-region and image
            crop = crop & cv::Rect2i(0, 0, src.width(), src.height());

            if(crop.size().area() == 0){
                throw std::invalid_argument("Crop area is zero.");
            }

            (src(crop)).copyTo(dst);

            *size_ptr = crop.size();
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}