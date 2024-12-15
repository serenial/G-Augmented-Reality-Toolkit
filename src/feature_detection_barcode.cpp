#include <opencv2/objdetect/barcode.hpp>

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

struct LV_QRElement_t{
    LV_StringHandle_t m_info, m_type;
    LV_ImagePointInt_t m_a,m_b,m_c,m_d;
};

using LV_BarcodeArrayHandle_t = LV_1DArrayHandle_t<LV_QRElement_t>;


#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_barcode_create_detector(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t proto_path_handle,
        LV_StringHandle_t model_path_handle,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            if(proto_path_handle.empty() || model_path_handle.empty()){
                volatile EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(edvr_ref_ptr, new cv::barcode::BarcodeDetector());
            }
            else{
                volatile EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(edvr_ref_ptr, new cv::barcode::BarcodeDetector(proto_path_handle, model_path_handle));
            }
            
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_barcode_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_BarcodeArrayHandle_t barcode_array_handle)
    {
        try
        {
            EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            std::vector<std::string> info, types;
            std::vector<cv::Point2i> points;

            barcode_detector->detectAndDecodeWithType(image, info, types, points);

            auto next_point = points.begin();
            auto next_type = types.begin();

            barcode_array_handle.copy_element_by_element_from(info,[&](auto from, auto to){
                to->m_info.copy_from(from);
                to->m_type.copy_from(*next_type++);
                to->m_a = *next_point++;
                to->m_b = *next_point++;
                to->m_c = *next_point++;
                to->m_d = *next_point++;
            }, [](auto to_deallocate){
                to_deallocate.m_info.dispose();
                to_deallocate.m_type.dispose();
            });            
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_barcode_detector_gradient_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        double* threshold,
        LV_BooleanPtr_t write
    )
    {
        try
        {
            EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(detector_ref_ptr);

            if(*write){
                barcode_detector->setGradientThreshold(*threshold);
            }

            *threshold = barcode_detector->getGradientThreshold();    
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_barcode_detector_downsampling_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        double* threshold,
        LV_BooleanPtr_t write
    )
    {
        try
        {
            EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(detector_ref_ptr);

            if(*write){
                barcode_detector->setDownsamplingThreshold(*threshold);
            }

            *threshold = barcode_detector->getDownsamplingThreshold();    
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

            G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_barcode_detector_scales(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_1DArrayHandle_t<float> scales_handle,
        LV_BooleanPtr_t write
    )
    {
        try
        {
            EDVRManagedObject<cv::barcode::BarcodeDetector> barcode_detector(detector_ref_ptr);

            if(*write){
                barcode_detector->setDetectorScales(scales_handle.as_vector<float>());
            }
            else{
                std::vector<float> scales;
                barcode_detector->getDetectorScales(scales);
                scales_handle.copy_memory_from(scales);
            }   
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}