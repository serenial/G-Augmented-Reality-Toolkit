#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_new(
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
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_new_argb_array(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_2DArrayHandle_t<uint32_t> array_handle,
        LV_BooleanPtr_t set_alpha_to_255)
    {
        try
        {
            // create an image instance - this will initialize all the persistant data and EDVR structure
            lv_image img(edvr_ref_ptr, array_handle.size(), true);

            if (array_handle.size().area() > 0)
            {
                if (*set_alpha_to_255)
                {

                    cv::Mat alpha(array_handle.size(), CV_8UC1, cv::Scalar(255));
                    // add the alpha channel but copy the image channels from the array
                    cv::mixChannels(std::begin({array_handle.as_cv_mat(CV_8UC4), alpha}), 2, img, 1, std::begin({0, 0, 1, 1, 2, 2, 4, 3}), 4);
                    return LV_ERR_noError;
                }

                array_handle.as_cv_mat(CV_8UC4).copyTo(img);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_new_greyscale_array(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_2DArrayHandle_t<uint8_t> array_handle)
    {
        try
        {
            // create an image instance - this will initialize all the persistant data and EDVR structure
            lv_image img(edvr_ref_ptr, array_handle.size(), false);

            if (array_handle.size().area() > 0)
            {
                array_handle.as_cv_mat(CV_8UC1).copyTo(img);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}