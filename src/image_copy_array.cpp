#include <stdexcept>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_PixelArrayHandle_t
    {
    public:
        cv::Size size() const
        {
            if (m_handle && (*m_handle))
            {
                return cv::Size{(*m_handle)->dims[1], (*m_handle)->dims[0]};
            }
            return cv::Size{0, 0};
        }

        cv::Mat as_cv_mat()
        {
            if (size().area() > 0 && (*m_handle)->data_ptr() != nullptr)
            {
                return cv::Mat(size(), CV_8UC((*m_handle)->dims[2]), (*m_handle)->data_ptr());
            }
            return cv::Mat();
        }

    private:
        LV_Handle_t<LV_Array_t<3, uint8_t>> m_handle;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_array_argb(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_2DArrayHandle_t<uint32_t> array_handle,
        LV_BooleanPtr_t set_alpha_to_255)
    {
        try
        {
            lv_image img(edvr_ref_ptr);

            img.ensure_sized_to_match(array_handle.size());

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
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_array_greyscale(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_2DArrayHandle_t<uint8_t> array_handle)
    {
        try
        {
            lv_image img(edvr_ref_ptr);

            img.ensure_sized_to_match(array_handle.size());

            if (array_handle.size().area() > 0)
            {
                array_handle.as_cv_mat(CV_8UC1).copyTo(img);
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_array_pixels(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_PixelArrayHandle_t array_handle)
    {
        try
        {
            lv_image img(edvr_ref_ptr);

            const cv::Mat from = array_handle.as_cv_mat();

            switch (from.channels())
            {
            case 3:
            {
                cv::cvtColor(from, img, cv::COLOR_RGB2BGRA);
            }
            break;
            case 4:
                from.copyTo(img);
                break;
            default:
                throw std::invalid_argument("The third dimension of the array (number of bytes per pixel) should either be three or four.");
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}