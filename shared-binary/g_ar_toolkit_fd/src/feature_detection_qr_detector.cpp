#include <vector>

#include <opencv2/objdetect.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_fd_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_QRCorners_t
    {
        LV_ImagePointFloat_t m_a, m_b, m_c, m_d;
        std::vector<cv::Point2f> as_vector() const
        {
            return std::vector<cv::Point2f>({m_a, m_b, m_c, m_d});
        }
    };

    struct LV_QRDetection_t
    {
        LV_StringHandle_t info;
        LV_QRCorners_t corners;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(edvr_ref_ptr, new cv::QRCodeDetector());
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_detect_single(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_Ptr_t<LV_QRCorners_t> detection,
        LV_BooleanPtr_t success_ptr)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<cv::Point2f> points;

            *success_ptr = qr_detector->detect(image, points);

            if (*success_ptr)
            {
                detection->m_a = points[0];
                detection->m_b = points[1];
                detection->m_c = points[2];
                detection->m_d = points[3];
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_detect_multiple(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_QRCorners_t> detections_handle)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<cv::Point2f> points;

            qr_detector->detectMulti(image, points);

            // detection points is one long list of points in sets of 4
            detections_handle.size_to_fit(points.size() >> 2);

            auto point = points.begin();
            for (auto &to : detections_handle)
            {
                to.m_a = *point++;
                to.m_b = *point++;
                to.m_c = *point++;
                to.m_d = *point++;
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_decode_single(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_Ptr_t<LV_QRCorners_t> detection,
        LV_StringHandle_t info,
        LV_BooleanPtr_t is_curved_ptr)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);
            lv_image image(src_edvr_ref_ptr);

            info.copy_from(*is_curved_ptr ? qr_detector->decodeCurved(image, detection->as_vector()) : qr_detector->decode(image, detection->as_vector()));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_decode_multiple(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_QRCorners_t> detections_handle,
        LV_1DArrayHandle_t<LV_StringHandle_t> infos_handle)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);
            lv_image image(src_edvr_ref_ptr);

            std::vector<std::string> infos;
            std::vector<cv::Point2f> points;

            for (const auto &d : detections_handle)
            {
                points.push_back(d.m_a);
                points.push_back(d.m_b);
                points.push_back(d.m_c);
                points.push_back(d.m_d);
            }

            qr_detector->decodeMulti(image, points, infos);

            infos_handle.copy_element_by_element_from(infos, [](const auto &from, auto to)
                                                      { to->copy_from(from); });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_detect_decode_multiple(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_QRDetection_t> detections_handle)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);
            lv_image image(src_edvr_ref_ptr);

            std::vector<std::string> infos;
            std::vector<cv::Point2f> points;

            qr_detector->detectAndDecodeMulti(image, infos, points);

            auto point = points.begin();

            detections_handle.copy_element_by_element_from(infos, [&](const auto &from, auto to)
                                                           {
                to->info.copy_from(from);
                to->corners.m_a = *point++;
                to->corners.m_b = *point++;
                to->corners.m_c = *point++;
                to->corners.m_d = *point++; });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_qr_detector_set_epsilon(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        double eps,
        LV_BooleanPtr_t set_y)
    {
        try
        {
            EDVRManagedObject<cv::QRCodeDetector> qr_detector(detector_ref_ptr);

            if (*set_y)
            {
                qr_detector->setEpsY(eps);
            }
            else
            {
                qr_detector->setEpsX(eps);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}