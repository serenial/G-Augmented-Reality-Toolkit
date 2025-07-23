#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    struct LV_EnumCVImageFlip_t : public LV_EnumCVInt_t
    {
        operator int() const
        {
            const int flip_codes[] =
                {
                    0, // flip-x
                    1, // flip-y
                    -1 // flip x and y
                };

            if (m_value < std::size(flip_codes))
            {
                return flip_codes[m_value];
            }

            throw std::out_of_range("The supplied value for the image flip type does not map to a valid OpenCV value.");
        }
    };
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_flip(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EnumCVImageFlip_t flip_mode)
    {
        try
        {

            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            dst.ensure_sized_to_match(src);

            cv::flip(src, dst, flip_mode);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}