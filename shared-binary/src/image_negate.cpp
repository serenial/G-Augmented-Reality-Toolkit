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
        LV_EDVRReferencePtr_t img_edvr_ref_ptr)
    {
        try
        {
            // operate in place
            lv_image img(img_edvr_ref_ptr);

            if (img.is_greyscale())
            {
                for (int row = 0; row < img.height(); row++)
                {
                    for (int col = 0; col < img.width(); col++)
                    {
                        img.at<uint8_t>(row, col) = uint8_t(255) - img.at<uchar>(row, col);
                    }
                }
            }
            else
            {
                for (int row = 0; row < img.height(); row++)
                {
                    for (int col = 0; col < img.width(); col++)
                    {
                        img.at<cv::Vec4b>(row, col) = cv::Vec4b(255, 255, 255, 255) - img.at<cv::Vec4b>(row, col);
                    }
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