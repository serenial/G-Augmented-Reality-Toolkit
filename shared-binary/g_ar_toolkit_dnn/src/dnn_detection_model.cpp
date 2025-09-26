#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <opencv2/dnn.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_dnn_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_DetectionCorners_t
    {
        LV_ImagePointInt_t m_top_left, m_bottom_right;
        LV_DetectionCorners_t &operator=(cv::Rect r)
        {

            m_top_left = r.tl();
            m_bottom_right = r.br();
            return *this;
        }
    };

    struct LV_Detection_t
    {
        LV_DetectionCorners_t corners;
        int32_t class_id;
        float confidence;
    };

    struct LV_Model_t
    {
        LV_StringHandle_t model_path_handle;
        LV_StringHandle_t config_path_handle;
        LV_ImageSize_t image_size;
        LV_Vec3Double_t mean;
        double scale;
        LV_Boolean_t use_RGB;
        LV_Boolean_t crop;
    };

    class LV_DnnTarget_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::dnn::Target() const
        {
            const cv::dnn::Target target_types[] =
                {
                    cv::dnn::Target::DNN_TARGET_CPU,
                    cv::dnn::Target::DNN_TARGET_OPENCL,
                    cv::dnn::Target::DNN_TARGET_OPENCL_FP16,
                    cv::dnn::Target::DNN_TARGET_MYRIAD,
                    cv::dnn::Target::DNN_TARGET_VULKAN,
                    cv::dnn::Target::DNN_TARGET_FPGA,
                    cv::dnn::Target::DNN_TARGET_CUDA,
                    cv::dnn::Target::DNN_TARGET_CUDA_FP16,
                    cv::dnn::Target::DNN_TARGET_HDDL,
                    cv::dnn::Target::DNN_TARGET_NPU,
                    cv::dnn::Target::DNN_TARGET_CPU_FP16};

            if (m_value < std::size(target_types))
            {
                return target_types[m_value];
            }

            throw std::out_of_range("The supplied value for the dnn-target type does not map to an OpenCV type.");
        }
    };

    class LV_DnnBackend_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::dnn::Backend() const
        {
            const cv::dnn::Backend backend_types[] =
                {
                    cv::dnn::Backend::DNN_BACKEND_DEFAULT,
                    cv::dnn::Backend::DNN_BACKEND_HALIDE,
                    cv::dnn::Backend::DNN_BACKEND_INFERENCE_ENGINE,
                    cv::dnn::Backend::DNN_BACKEND_OPENCV,
                    cv::dnn::Backend::DNN_BACKEND_VKCOM,
                    cv::dnn::Backend::DNN_BACKEND_CUDA,
                    cv::dnn::Backend::DNN_BACKEND_WEBNN,
                    cv::dnn::Backend::DNN_BACKEND_TIMVX,
                    cv::dnn::Backend::DNN_BACKEND_CANN};

            if (m_value < std::size(backend_types))
            {
                return backend_types[m_value];
            }

            throw std::out_of_range("The supplied value for the dnn-backend type does not map to an OpenCV type.");
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    // create a class to store the cv::dnn::DetectionModel as it's lifetime mechanics seems a bit weird otherwise
    struct dnn_model_t
    {
        cv::dnn::DetectionModel m_model;
        dnn_model_t(const LV_Model_t &model_parameters, const LV_DnnTarget_t target, const LV_DnnBackend_t backend) : m_model(model_parameters.model_path_handle, model_parameters.config_path_handle)
        {
            m_model.setInputParams(
                model_parameters.scale,
                model_parameters.image_size,
                model_parameters.mean,
                model_parameters.use_RGB,
                model_parameters.crop);

            m_model.setPreferableTarget(target);
            m_model.setPreferableBackend(backend);
        }
    };

}

extern "C"
{
    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_detection_model_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        const LV_Ptr_t<LV_Model_t> model_parameters,
        LV_DnnTarget_t target,
        LV_DnnBackend_t backend,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            EDVRManagedObject<dnn_model_t>(edvr_ref_ptr, new dnn_model_t(*model_parameters, target, backend));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_detection_model_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_edvr_ref_ptr,
        LV_EDVRReferencePtr_t image_edvr_ref_ptr,
        float confidence_threshold,
        float nms_threshold,
        LV_1DArrayHandle_t<LV_Detection_t> detections_handle)
    {
        try
        {
            EDVRManagedObject<dnn_model_t> model(detector_edvr_ref_ptr);
            lv_image src(image_edvr_ref_ptr);

            cv::Mat frame;

            cv::cvtColor(src, frame, src.is_bgra() ? cv::COLOR_BGRA2BGR : cv::COLOR_GRAY2BGR);

            std::vector<int> class_ids;
            std::vector<float> confidences;
            std::vector<cv::Rect> boxes;

            model->m_model.detect(frame, class_ids, confidences, boxes, confidence_threshold, nms_threshold);

            detections_handle.size_to_fit(class_ids.size());

            for (int i = 0; i < class_ids.size(); i++)
            {
                detections_handle[i].class_id = class_ids[i];
                detections_handle[i].confidence = confidences[i];
                detections_handle[i].corners = boxes[i];
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}