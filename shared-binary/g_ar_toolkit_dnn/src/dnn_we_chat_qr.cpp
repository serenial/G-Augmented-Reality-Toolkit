#include <opencv2/wechat_qrcode.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"

#include "g_ar_toolkit_dnn_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_WeChatQRElement_t
    {
        LV_StringHandle_t m_info;
        LV_ImagePointFloat_t m_a, m_b, m_c, m_d;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_we_chat_qr_create_detector(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t detector_proto_handle,
        LV_StringHandle_t detector_model_handle,
        LV_StringHandle_t sr_proto_handle,
        LV_StringHandle_t sr_mode_handle,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            EDVRManagedObject<cv::wechat_qrcode::WeChatQRCode> qr_detector(edvr_ref_ptr, new cv::wechat_qrcode::WeChatQRCode(detector_proto_handle, detector_model_handle, sr_proto_handle, sr_mode_handle));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_we_chat_qr_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_WeChatQRElement_t> qr_code_array_handle)
    {
        try
        {
            EDVRManagedObject<cv::wechat_qrcode::WeChatQRCode> qr_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<cv::Mat> points;

            auto info = qr_detector->detectAndDecode(image, points);

            auto next_point = points.begin();

            qr_code_array_handle.copy_element_by_element_from(info, [&](const auto &from, auto to)
                                                              {
                to->m_info.copy_from(from);
                auto points = *next_point++; // copy and increment ready for next time
                to->m_a.copy_from(points.ptr<float>(0,0));
                to->m_b.copy_from(points.ptr<float>(1,0));
                to->m_c.copy_from(points.ptr<float>(2,0));
                to->m_d.copy_from(points.ptr<float>(3,0)); });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_we_chat_qr_detector_scale_factor(
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
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}