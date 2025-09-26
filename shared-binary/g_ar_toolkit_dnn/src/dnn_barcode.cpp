#include <opencv2/objdetect/barcode.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_dnn_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_BarcodeElement_t
    {
        LV_StringHandle_t m_info, m_type;
        LV_ImagePointInt_t m_a, m_b, m_c, m_d;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

// create an object to hold the bardcode detector to work with openCV's lifetime and ownership semantics
struct barcode_detector_t{
    cv::barcode::BarcodeDetector m_barcode_detector;
    barcode_detector_t(LV_StringHandle_t sr_proto_handle, LV_StringHandle_t sr_model_handle): m_barcode_detector(sr_proto_handle, sr_model_handle) {}
};

}

extern "C"
{
    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_barcode_create_detector(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t sr_proto_handle,
        LV_StringHandle_t sr_model_handle,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            EDVRManagedObject<barcode_detector_t> barcode_detector(edvr_ref_ptr, new barcode_detector_t(sr_proto_handle, sr_model_handle));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_barcode_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_BarcodeElement_t> barcode_array_handle)
    {
        try
        {
            EDVRManagedObject<barcode_detector_t> bd(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<std::string> info, types;
            std::vector<cv::Point2i> points;

            bd->m_barcode_detector.detectAndDecodeWithType(image, info, types, points);

            auto next_point = points.begin();
            auto next_type = types.begin();

            barcode_array_handle.copy_element_by_element_from(info, [&](auto from, auto to)
                                                              {
                to->m_info.copy_from(from);
                to->m_type.copy_from(*next_type++);
                to->m_a = *next_point++;
                to->m_b = *next_point++;
                to->m_c = *next_point++;
                to->m_d = *next_point++; });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_barcode_detector_gradient_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        double *threshold,
        LV_BooleanPtr_t write)
    {
        try
        {
            EDVRManagedObject<barcode_detector_t> bd(detector_ref_ptr);

            if (*write)
            {
                bd->m_barcode_detector.setGradientThreshold(*threshold);
            }

            *threshold = bd->m_barcode_detector.getGradientThreshold();
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_barcode_detector_downsampling_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        double *threshold,
        LV_BooleanPtr_t write)
    {
        try
        {
            EDVRManagedObject<barcode_detector_t> bd(detector_ref_ptr);

            if (*write)
            {
                bd->m_barcode_detector.setDownsamplingThreshold(*threshold);
            }

            *threshold = bd->m_barcode_detector.getDownsamplingThreshold();
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_barcode_detector_scales(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_1DArrayHandle_t<float> scales_handle,
        LV_BooleanPtr_t write)
    {
        try
        {
            EDVRManagedObject<barcode_detector_t> bd(detector_ref_ptr);

            if (*write)
            {
                bd->m_barcode_detector.setDetectorScales(scales_handle.as_vector<float>());
            }
            else
            {
                std::vector<float> scales;
                bd->m_barcode_detector.getDetectorScales(scales);
                scales_handle.copy_memory_from(scales);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}