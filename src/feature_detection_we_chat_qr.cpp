#include <opencv2/wechat_qrcode.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_QRElement_t
    {
        LV_StringHandle_t m_info;
        LV_ImagePointFloat_t m_a, m_b, m_c, m_d;
    };

    using LV_BarcodeArrayHandle_t = LV_1DArrayHandle_t<LV_QRElement_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_we_chat_qr_create_detector(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<LV_StringHandle_t> paths,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {

            volatile EDVRManagedObject<cv::wechat_qrcode::WeChatQRCode> qr_detector(edvr_ref_ptr, new cv::wechat_qrcode::WeChatQRCode(paths[0], paths[1], paths[2], paths[3]));
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_we_chat_qr_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_BarcodeArrayHandle_t barcode_array_handle)
    {
        try
        {
            EDVRManagedObject<cv::wechat_qrcode::WeChatQRCode> qr_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<cv::Mat> points;

            auto info = qr_detector->detectAndDecode(image, points);

            auto next_point = points.begin();

            barcode_array_handle.copy_element_by_element_from(info, [&](auto from, auto to)
                                                              {
                to->m_info.copy_from(from);
                auto points = *next_point++;
                to->m_a= &points.at<float>(0,0);
                to->m_b= &points.at<float>(1,0);
                to->m_c= &points.at<float>(2,0);
                to->m_d= &points.at<float>(3,0); }, [](auto to_deallocate)
                                                              { to_deallocate.m_info.dispose(); });
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_we_chat_qr_detector_scale_factor(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        float *scale_factor,
        LV_BooleanPtr_t write)
    {
        try
        {
            EDVRManagedObject<cv::wechat_qrcode::WeChatQRCode> qr_detector(detector_ref_ptr);

            if (*write)
            {
                qr_detector->setScaleFactor(*scale_factor);
            }
            else
            {
                *scale_factor = qr_detector->getScaleFactor();
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}