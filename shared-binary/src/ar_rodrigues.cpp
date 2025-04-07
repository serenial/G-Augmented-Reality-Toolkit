#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_rodrigues_rvec_to_rmat(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_RVecPtr_t rvec_ptr,
        LV_3x3MatrixPtr_t rmat)
    {
        try
        {
            cv::Rodrigues(rvec_ptr->to_vec3d(), rmat->as_cv_mat());
        }
        catch (...)
        {
error_cluster_ptr.copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_rodrigues_rmat_to_rvec(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_3x3MatrixPtr_t rmat,
        LV_RVecPtr_t rvec_ptr)
    {
        try
        {
            cv::Rodrigues(rmat->as_cv_mat(), rvec_ptr->to_vec3d());
        }
        catch (...)
        {
error_cluster_ptr.copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}