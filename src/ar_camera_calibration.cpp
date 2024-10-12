#include <functional>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_termination_criteria.hpp"
#include "g_ar_toolkit/lv_interop/lv_point_correspondence.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    // define a type which is an LV 1d-array of rvecs or tvecs
    // and add a method which converts those vecs to non-owning cv:Mats
    // which point directly to the LV-managed memory
    struct LV_1DVecArrayHandle_t : public LV_1DArrayHandle_t<LV_Vec3Double_t>
    {
        std::vector<cv::Mat> as_cv_mats()
        {
            std::vector<cv::Mat> out;
            out.reserve(size());

            for(auto &v : *this){
                out.push_back(v);
            }

            return cv::Mat();
        }
    };

    using LV_1DRVecArrayHandle_t = LV_1DVecArrayHandle_t;
    using LV_1DTVecArrayHandle_t = LV_1DVecArrayHandle_t;
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_camera_calibration(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PointCorrespondencesArrayHandle_t point_correspondence_handle,
        LV_ImageSizePtr_t size_ptr,
        int32_t fixed_point_index,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_1DRVecArrayHandle_t rvecs_handle,
        LV_1DTVecArrayHandle_t tvecs_handle,
        LV_1DArrayHandle_t<double> rms_handle_ptr,
        LV_1DArrayHandle_t<LV_EnumCVCameraCalibrationFlags_t> flags_array_handle,
        LV_Ptr_t<LV_TerminationCritera_t> term_crit_ptr)
    {
        try
        {
            // these argument types are OutputArray-typed so any values have to be copied into LabVIEW managed memory
            std::vector<double> newObjPoints, stdDeviationsIntrinsics, stdDeviationsExtrinsics, stdDeviationsObjPoints, rms;

            cv::calibrateCameraRO(
                point_correspondence_handle.object_points_vecs(),
                point_correspondence_handle.image_points_vecs(),
                *size_ptr,
                fixed_point_index,
                camera_matrix_ptr->as_cv_mat(),
                camera_distortion_ptr->as_cv_mat(5),
                rvecs_handle.as_vector<cv::Mat>(),
                tvecs_handle.as_vector<cv::Mat>(),
                newObjPoints,
                stdDeviationsIntrinsics,
                stdDeviationsExtrinsics,
                stdDeviationsObjPoints,
                rms,
                LV_EnumCVInt_t::combine(flags_array_handle),
                *term_crit_ptr);

            rms_handle_ptr.copy_from(rms);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}