#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"

#include "g_ar_toolkit_ar_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_image_undistort(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            cv::undistort(src, dst, camera_matrix_ptr->as_cv_mat(), camera_distortion_ptr->as_cv_mat());
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}