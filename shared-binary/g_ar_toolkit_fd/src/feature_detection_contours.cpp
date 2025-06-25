#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

#include "g_ar_toolkit_fd_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_EnumContourApproximationMode_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::ContourApproximationModes() const
        {
            const cv::ContourApproximationModes modes[] = {
                cv::ContourApproximationModes::CHAIN_APPROX_NONE,
                cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE,
                cv::ContourApproximationModes::CHAIN_APPROX_TC89_L1,
                cv::ContourApproximationModes::CHAIN_APPROX_TC89_KCOS};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the Contour Approximation Mode does not map to a valid OpenCV value.");
        }
    };

    class LV_EnumContourRetrievalMode_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::RetrievalModes() const
        {
            const cv::RetrievalModes modes[] = {
                cv::RetrievalModes::RETR_EXTERNAL,
                cv::RetrievalModes::RETR_LIST,
                cv::RetrievalModes::RETR_CCOMP,
                cv::RetrievalModes::RETR_TREE,
                cv::RetrievalModes::RETR_FLOODFILL};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the Contour Retrieval Mode does not map to a valid OpenCV value.");
        }
    };

    class LV_ContourHierarchyElement_t
    {
    public:
        LV_ContourHierarchyElement_t &operator=(cv::Vec4i vec)
        {
            m_v0 = vec[0];
            m_v1 = vec[1];
            m_v2 = vec[2];
            m_v3 = vec[3];
            return *this;
        }

    private:
        int32_t m_v0, m_v1, m_v2, m_v3;
    };

    struct LV_ContourElement_t
    {
        LV_1DArrayHandle_t<LV_ImagePointInt_t> points;
        LV_ContourHierarchyElement_t hierarchy;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_FD_EXPORT LV_MgErr_t g_ar_tk_fd_find_contours(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EnumContourRetrievalMode_t retrieval_mode,
        LV_EnumContourApproximationMode_t contour_mode,
        LV_1DArrayHandle_t<LV_ContourElement_t> contours_handle)
    {
        try
        {
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;

            lv_image src(src_edvr_ref_ptr);

            cv::findContours(src, contours, hierarchy, retrieval_mode, contour_mode);

            size_t index = 0;
            contours_handle.copy_element_by_element_from(contours, [&](auto from, auto to)
                                                         {
                to->points.copy_element_by_element_from(from);
                to->hierarchy = hierarchy[index];
                ++index; });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}