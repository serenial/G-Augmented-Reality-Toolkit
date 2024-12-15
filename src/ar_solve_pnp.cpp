#include <functional>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_point_correspondence.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    struct LV_EnumCVSolvePnPFlags_t : public LV_EnumCVInt_t
    {
        operator int() const
        {
            const int flags[] = {

                cv::SOLVEPNP_ITERATIVE,
                cv::SOLVEPNP_EPNP,
                cv::SOLVEPNP_P3P,
                cv::SOLVEPNP_AP3P,
                cv::SOLVEPNP_IPPE,
                cv::SOLVEPNP_IPPE_SQUARE,
                cv::SOLVEPNP_SQPNP};

            if (m_value < std::size(flags))
            {
                return flags[m_value];
            }

            throw std::out_of_range("The supplied value for the solve-PnP flag does not map to a valid OpenCV value.");
        }
    };
#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_solve_pnp(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PointCorrespondencePtr_t point_correspondence_ptr,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_RVecPtr_t rvec_ptr,
        LV_TVecPtr_t tvec_ptr,
        LV_BooleanPtr_t use_intrinsic,
        LV_EnumCVSolvePnPFlags_t flag)
    {
        try
        {
            // solvePnP is expecting Vec3ds to operate on (in place cv::Mats will not suffice) 
            // copy incoming values if using intrinsic guess

            cv::Vec3d rvec, tvec;

            if (*use_intrinsic)
            {
                rvec = rvec_ptr->to_vec3d();
                tvec = tvec_ptr->to_vec3d();
            }

            cv::solvePnP(
                point_correspondence_ptr->object_points_vec(),
                point_correspondence_ptr->image_points_vec(),
                camera_matrix_ptr->as_cv_mat(),
                camera_distortion_ptr->as_cv_mat(),
                rvec,
                tvec,
                *use_intrinsic,
                flag);

            *rvec_ptr = rvec;
            *tvec_ptr = tvec;
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}