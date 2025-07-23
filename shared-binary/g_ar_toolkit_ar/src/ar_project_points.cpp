#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_point_correspondence.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_ar_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_project_points(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<LV_ObjectPointFloat_t> object_points_handle,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_RVecPtr_t rvec_ptr,
        LV_TVecPtr_t tvec_ptr,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> image_points_handle)
    {
        try
        {

            std::vector<cv::Point2f> image_points;
            image_points.reserve(object_points_handle.size());

            cv::projectPoints(
                object_points_handle.as_vector<cv::Point3f>(),
                rvec_ptr->to_vec3d(),
                tvec_ptr->to_vec3d(),
                camera_matrix_ptr->as_cv_mat(),
                camera_distortion_ptr->as_cv_mat(),
                image_points);

            image_points_handle.copy_element_by_element_from(image_points);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}