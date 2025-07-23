#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_point_correspondence.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

#include "g_ar_toolkit_ar_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    struct LV_EnumCVSolvePnPFlags_t : public LV_EnumCVInt_t
    {
        operator int() const;
    };

    class LV_UsacParams_t
    {
    public:
        LV_UsacParams_t() = delete;
        operator cv::UsacParams() const;

    private:
        double m_confidence, m_threshold;
        uint16_t m_final_polish_iter, m_lo_iter, m_lo_sample_size, m_max_iter;
        int32_t m_random_generator_state;
        uint8_t m_polish_mode, m_lo_method, m_neighbor_search_method, m_sampler, m_score;
        LV_Boolean_t m_is_parallel;
    };

    using LV_UsacParamsPtr_t = LV_Ptr_t<LV_UsacParams_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_solve_pnp(
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
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_solve_pnp_usac(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PointCorrespondencePtr_t point_correspondence_ptr,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_UsacParamsPtr_t usac_params_ptr,
        LV_RVecPtr_t rvec_ptr,
        LV_TVecPtr_t tvec_ptr,
        LV_1DArrayHandle_t<int32_t> inliers_handle)
    {
        try
        {
            // solvePnP is expecting Vec3ds to operate on (in place cv::Mats will not suffice)
            cv::Vec3d rvec, tvec;

            std::vector<int32_t> inliers;

            inliers.reserve(point_correspondence_ptr->number_of_points());

            cv::solvePnPRansac(
                point_correspondence_ptr->object_points_vec(),
                point_correspondence_ptr->image_points_vec(),
                camera_matrix_ptr->as_cv_mat(),
                camera_distortion_ptr->as_cv_mat(),
                rvec,
                tvec,
                inliers,
                *usac_params_ptr);

            *rvec_ptr = rvec;
            *tvec_ptr = tvec;

            inliers_handle.copy_memory_from(inliers);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_find_homography_usac(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> source_handle,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> destination_handle,
        LV_UsacParamsPtr_t usac_params_ptr,
        LV_3x3MatrixPtr_t h_mat_ptr,
        LV_BooleanPtr_t not_found_ptr,
        LV_1DArrayHandle_t<int32_t> inliers_handle)
    {
        try
        {
            cv::Mat mask, h = cv::findHomography(
                              source_handle.to_vector<cv::Point2f>(),
                              destination_handle.to_vector<cv::Point2f>(),
                              mask,
                              *usac_params_ptr);

            auto x = mask.size();

            *h_mat_ptr = h;
            *not_found_ptr = h.empty();

            std::vector<int32_t> inliers;
            inliers.reserve(mask.rows);

            for (int r = 0; r < mask.rows; r++)
            {
                if (mask.at<char>(r, 0))
                {
                    inliers.push_back(r);
                }
            }

            inliers_handle.copy_element_by_element_from(inliers);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}

LV_EnumCVSolvePnPFlags_t::operator int() const
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

LV_UsacParams_t::operator cv::UsacParams() const
{
    const cv::PolishingMethod polishing_modes[] = {
        cv::PolishingMethod::NONE_POLISHER,
        cv::PolishingMethod::LSQ_POLISHER,
        cv::PolishingMethod::MAGSAC,
        cv::PolishingMethod::COV_POLISHER};

    if (m_polish_mode >= std::size(polishing_modes))
    {
        throw std::out_of_range("The supplied value for the polishing mode does not map to a valid OpenCV value.");
    }

    const cv::LocalOptimMethod lo_methods[] = {
        cv::LocalOptimMethod::LOCAL_OPTIM_NULL,
        cv::LocalOptimMethod::LOCAL_OPTIM_INNER_LO,
        cv::LocalOptimMethod::LOCAL_OPTIM_INNER_AND_ITER_LO,
        cv::LocalOptimMethod::LOCAL_OPTIM_GC,
        cv::LocalOptimMethod::LOCAL_OPTIM_SIGMA};

    if (m_lo_method >= std::size(lo_methods))
    {
        throw std::out_of_range("The supplied value for the local optimization method does not map to a valid OpenCV value.");
    }

    const cv::NeighborSearchMethod neighbor_search_methods[] = {
        cv::NeighborSearchMethod::NEIGH_FLANN_KNN,
        cv::NeighborSearchMethod::NEIGH_GRID,
        cv::NeighborSearchMethod::NEIGH_FLANN_RADIUS};

    if (m_neighbor_search_method >= std::size(neighbor_search_methods))
    {
        throw std::out_of_range("The supplied value for the neighbor search method does not map to a valid OpenCV value.");
    }

    const cv::SamplingMethod sampling_methods[] = {
        cv::SamplingMethod::SAMPLING_UNIFORM,
        cv::SamplingMethod::SAMPLING_PROGRESSIVE_NAPSAC,
        cv::SamplingMethod::SAMPLING_NAPSAC,
        cv::SamplingMethod::SAMPLING_PROSAC};

    if (m_sampler >= std::size(sampling_methods))
    {
        throw std::out_of_range("The supplied value for the sampling method does not map to a valid OpenCV value.");
    }

    const cv::ScoreMethod score_methods[] = {
        cv::ScoreMethod::SCORE_METHOD_RANSAC,
        cv::ScoreMethod::SCORE_METHOD_MSAC,
        cv::ScoreMethod::SCORE_METHOD_MAGSAC,
        cv::ScoreMethod::SCORE_METHOD_LMEDS};

    if (m_score >= std::size(score_methods))
    {
        throw std::out_of_range("The supplied value for the score method does not map to a valid OpenCV value.");
    }

    cv::UsacParams params;

    params.confidence = m_confidence;
    params.threshold = m_threshold;
    params.isParallel = m_is_parallel;
    params.loIterations = m_lo_iter;
    params.loSampleSize = m_lo_sample_size;
    params.final_polisher_iterations = m_final_polish_iter;
    params.randomGeneratorState = m_random_generator_state;
    params.maxIterations = m_max_iter;

    params.final_polisher = polishing_modes[m_polish_mode];
    params.loMethod = lo_methods[m_lo_method];
    params.neighborsSearch = neighbor_search_methods[m_neighbor_search_method];
    params.sampler = sampling_methods[m_sampler];
    params.score = score_methods[m_score];

    return params;
}