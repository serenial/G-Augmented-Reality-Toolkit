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

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_DetectionCorners_t
    {
        LV_ImagePointFloat_t bottom_left, bottom_right, top_right, top_left;
    };

    struct LV_Detection_t
    {
        LV_DetectionCorners_t corners;
        float confidence;
    };

    struct LV_TextDetectModel_t
    {
        LV_StringHandle_t model_data;
        LV_StringHandle_t model_config;
        LV_ImageSize_t image_size;
        LV_Vec3Double_t mean;
        double scale;
        LV_Boolean_t use_RGB;
    };

    struct LV_TextRecognizeModel_t
    {
        LV_StringHandle_t model_data;
        LV_StringHandle_t model_config;
        LV_StringHandle_t vocab;
        uint8_t mode;
        LV_ImageSize_t image_size;
        LV_Vec3Double_t mean;
        double scale;
        LV_Boolean_t use_RGB;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_dnn_text_create_db(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        const LV_Ptr_t<LV_TextDetectModel_t> model,
        uint16_t max_candidates,
        float binary_threshold,
        float poly_threshold,
        double unclip_ratio,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {

            auto text_detector = new cv::dnn::TextDetectionModel_DB(model->model_data, model->model_config);

            text_detector->setMaxCandidates(max_candidates)
                .setBinaryThreshold(binary_threshold)
                .setPolygonThreshold(poly_threshold)
                .setUnclipRatio(unclip_ratio)
                .setInputSize(model->image_size)
                .setInputMean(model->mean)
                .setInputScale(model->scale)
                .setInputSwapRB(model->use_RGB);

            EDVRManagedObject<cv::dnn::TextDetectionModel>(edvr_ref_ptr, text_detector);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_dnn_text_create_east(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        const LV_Ptr_t<LV_TextDetectModel_t> model,
        float conf_threshold,
        float nms_threshold,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {

            auto text_detector = new cv::dnn::TextDetectionModel_EAST(model->model_data, model->model_config);

            text_detector->setConfidenceThreshold(conf_threshold)
                .setNMSThreshold(nms_threshold)
                .setInputSize(model->image_size)
                .setInputMean(model->mean)
                .setInputScale(model->scale)
                .setInputSwapRB(model->use_RGB);

            EDVRManagedObject<cv::dnn::TextDetectionModel>(edvr_ref_ptr, text_detector);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_dnn_text_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_edvr_ref_ptr,
        LV_EDVRReferencePtr_t image_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_Detection_t> detections_handle)
    {
        try
        {
            EDVRManagedObject<cv::dnn::TextDetectionModel> text_detector(detector_edvr_ref_ptr);
            lv_image src(image_edvr_ref_ptr);

            cv::Mat frame;

            cv::cvtColor(src, frame, src.is_bgra() ? cv::COLOR_BGRA2BGR : cv::COLOR_GRAY2BGR);

            std::vector<std::vector<cv::Point>> quads;
            std::vector<float> confidences;

            text_detector->detect(frame, quads, confidences);

            detections_handle.size_to_fit(quads.size());

            for (int i = 0; i < quads.size() && i < confidences.size(); i++)
            {
                detections_handle[i].corners.bottom_left = quads[i][0];
                detections_handle[i].corners.top_left = quads[i][1];
                detections_handle[i].corners.top_right = quads[i][2];
                detections_handle[i].corners.bottom_right = quads[i][3];

                detections_handle[i].confidence = confidences[i];
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_dnn_text_create_recognizer(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        const LV_Ptr_t<LV_TextRecognizeModel_t> model,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            auto recognizer = new cv::dnn::TextRecognitionModel(model->model_data, model->model_config);

            std::vector<std::string> tokens;
            std::istringstream iss(model->vocab);
            std::string token;

            while (iss >> token)
            {
                tokens.push_back(token);
            }

            recognizer->setVocabulary(tokens)
                .setInputMean(model->mean)
                .setInputScale(model->scale)
                .setInputSize(model->image_size)
                .setInputSwapRB(model->use_RGB);

            recognizer->setDecodeType(model->mode ? "CTC-prefix-beam-search" : "CTC-greedy");

            EDVRManagedObject<cv::dnn::TextRecognitionModel>(edvr_ref_ptr, recognizer);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_dnn_text_recognize(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t recognizer_edvr_ref_ptr,
        LV_EDVRReferencePtr_t image_edvr_ref_ptr,
        LV_StringHandle_t result)
    {
        try
        {
            EDVRManagedObject<cv::dnn::TextRecognitionModel> recognizer(recognizer_edvr_ref_ptr);

            lv_image src(image_edvr_ref_ptr);

            cv::Mat frame;

            cv::cvtColor(src, frame, src.is_bgra() ? cv::COLOR_BGRA2BGR : cv::COLOR_GRAY2BGR);

            result.copy_from(recognizer->recognize(frame));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}