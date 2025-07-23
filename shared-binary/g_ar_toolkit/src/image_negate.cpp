#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_negate_in_place(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_BooleanPtr_t use_bitwise_not)
    {
        try
        {

            // operate in place
            lv_image img(img_edvr_ref_ptr);

            if (use_bitwise_not)
            {
                cv::bitwise_not(img, img);
            }
            else
            {

                if (img.is_greyscale())
                {
                    *img = 255 - img.mat();
                }
                else
                {
                    *img = cv::Vec4b(255, 255, 255, 255) - img.mat();
                }
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}