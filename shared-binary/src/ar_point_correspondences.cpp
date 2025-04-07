#include <pcl/registration/correspondence_estimation.h>
#include <pcl/registration/correspondence_rejection_sample_consensus.h>
#include <pcl/registration/correspondence_rejection_one_to_one.h>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_PointCloudArrayHandle_t : public LV_1DArrayHandle_t<LV_ObjectPointFloat_t>
    {
    public:
        LV_PointCloudArrayHandle_t() = delete;
        std::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> to_point_cloud()
        {
            pcl::PointCloud<pcl::PointXYZ>::Ptr output(new pcl::PointCloud<pcl::PointXYZ>(LV_1DArrayHandle_t<LV_ObjectPointFloat_t>::size(), 1));

            auto from = begin();
            auto to = output->begin();

            for (; from < end() && to < output->end();)
            {
                *to++ = *from++;
            }

            return output;
        }
    };

    class LV_CorrespondencePair_t
    {
        int32_t m_match, m_query;

    public:
        LV_CorrespondencePair_t &operator=(const pcl::Correspondence c)
        {
            m_match = c.index_match;
            m_query = c.index_query;
            return *this;
        }
    };

    class LV_CorrespondenceFinder_Parameters_t
    {
        LV_Boolean_t refine;
        int32_t max_iterations;
        double inlier_threshold;

    public:
        template <class PointT>
        void apply_parameters_to(pcl::registration::CorrespondenceRejectorSampleConsensus<PointT> rejector) const
        {
            if (!std::isnan(inlier_threshold))
            {
                rejector.setInlierThreshold(inlier_threshold);
            }
            if (max_iterations > 0)
            {
                rejector.setMaximumIterations(max_iterations);
            }

            rejector.setRefineModel(refine);
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_point_correspondences(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PointCloudArrayHandle_t source_points_handle,
        LV_PointCloudArrayHandle_t target_points_handle,
        LV_Ptr_t<LV_CorrespondenceFinder_Parameters_t> params_ptr,
        LV_1DArrayHandle_t<LV_CorrespondencePair_t> correspondences_handle)
    {
        try
        {
            auto source = source_points_handle.to_point_cloud();
            auto target = target_points_handle.to_point_cloud();

            // Compute initial correspondences
            pcl::registration::CorrespondenceEstimation<pcl::PointXYZ, pcl::PointXYZ> est;
            est.setInputSource(source);
            est.setInputTarget(target);
            pcl::CorrespondencesPtr all_correspondences(new pcl::Correspondences);
            est.determineCorrespondences(*all_correspondences);

            // Set up the RANSAC correspondence rejector
            pcl::registration::CorrespondenceRejectorSampleConsensus<pcl::PointXYZ> rejector_consensus;
            rejector_consensus.setInputSource(source);
            rejector_consensus.setInputTarget(target);
            params_ptr->apply_parameters_to(rejector_consensus);

            // Apply the rejector to filter correspondences
            pcl::CorrespondencesPtr inlier_correspondences(new pcl::Correspondences);
            rejector_consensus.getRemainingCorrespondences(*all_correspondences, *inlier_correspondences);

            pcl::registration::CorrespondenceRejectorOneToOne one_to_one_rejector;
            pcl::CorrespondencesPtr one_to_one_correspondences(new pcl::Correspondences);
            one_to_one_rejector.getRemainingCorrespondences(*inlier_correspondences, *one_to_one_correspondences);

            correspondences_handle.size_to_fit(one_to_one_correspondences->size());

            auto pair_ptr = correspondences_handle.begin();

            for (const auto &correspondence : *one_to_one_correspondences)
            {
                *pair_ptr++ = correspondence;
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}