#ifndef G_AR_TK__INTEROP_LV_POINT_CORRESPONDENCE_HPP_
#define G_AR_TK__INTEROP_LV_POINT_CORRESPONDENCE_HPP_

#include <vector>
#include <utility>

#include <opencv2/core.hpp>

#include "./lv_types.hpp"
#include "./lv_array_1d.hpp"
#include "./lv_vec_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
#include "./set_packing.hpp"


        using LV_ImagePointsFloatArray_t = LV_1DArrayHandle_t<LV_ImagePointFloat_t>;
        using LV_ObjectPointsFloatArray_t = LV_1DArrayHandle_t<LV_ObjectPointFloat_t>;

        class LV_PointCorrespondenceArray_t
        {
            private:
            LV_ObjectPointsFloatArray_t m_object_points_handle;
            LV_ImagePointsFloatArray_t m_image_points_handle;
            public:
            LV_PointCorrespondenceArray_t() = delete;
            std::vector<cv::Point2f> image_points_vec() const;
            std::vector<cv::Point3f> object_points_vec() const;
        };

        class LV_PointCorrespondencesArrayHandle_t{
            private:
            LV_1DArrayHandle_t<LV_PointCorrespondenceArray_t> m_point_sets;
            public:
            LV_PointCorrespondencesArrayHandle_t() = delete;
            std::vector<std::vector<cv::Point2f>> image_points_vecs() const;
            std::vector<std::vector<cv::Point3f>> object_points_vecs() const;
        };

#include "./reset_packing.hpp"
    }
}

#endif // G_AR_TK__INTEROP_LV_POINT_CORRESPONDENCE_HPP_