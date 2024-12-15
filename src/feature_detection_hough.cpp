#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_EnumHoughMode_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::HoughModes() const
        {
            const cv::HoughModes modes[] = {
                cv::HoughModes::HOUGH_STANDARD,
                cv::HoughModes::HOUGH_PROBABILISTIC,
                cv::HoughModes::HOUGH_MULTI_SCALE,
                cv::HoughModes::HOUGH_GRADIENT,
                cv::HoughModes::HOUGH_GRADIENT_ALT,
            };
            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the Hough Mode does not map to a valid OpenCV value.");
        }
    };
    class LV_CircleElement_t
    {
        public:
        LV_CircleElement_t &operator=(cv::Vec4f vector)
        {
            m_x = vector[0];
            m_y = vector[1];
            m_radius = vector[2];
            m_votes = vector[3];
            return *this;
        }
        private:
        double m_x, m_y, m_radius, m_votes;
    };

    using LV_CirclesArrayHandle_t = LV_1DArrayHandle_t<LV_CircleElement_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_hough_circles(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EnumHoughMode_t hough_mode,
        double dp,
        double min_distance,
        double param1,
        double param2,
        int32_t min_radius,
        int32_t max_radius,
        LV_CirclesArrayHandle_t circles_handle)
    {
        try
        {
            std::vector<cv::Vec4f> circles;

            lv_image src(src_edvr_ref_ptr);

            cv::HoughCircles(src, circles, hough_mode, dp, min_distance, param1, param2, min_radius, max_radius);

            circles_handle.copy_element_by_element_from(circles);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}