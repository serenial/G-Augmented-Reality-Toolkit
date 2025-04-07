#pragma once

#include <opencv2/core.hpp>
#include <eigen3/Eigen/Core>

#include "g_ar_toolkit/lv_interop/lv_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
#include "./set_packing.hpp"

using eigen_map_3x3_t = Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>>;

        class LV_3x3Matrix_t
        {
        public:
            operator cv::Mat();
            operator eigen_map_3x3_t();
            cv::Mat as_cv_mat();
            eigen_map_3x3_t as_eigen_map();
            LV_3x3Matrix_t& operator=(const Eigen::Matrix3d&);
            LV_3x3Matrix_t& operator=(const cv::Mat&);

        private:
            double m_value[9];
        };

        using LV_3x3MatrixPtr_t = LV_Ptr_t<LV_3x3Matrix_t>;

        class LV_4x4Matrix_t
        {
        public:
            LV_4x4Matrix_t& operator=(const Eigen::Matrix4f&);

        private:
            double m_value[16];
        };

        using LV_4x4MatrixPtr_t = LV_Ptr_t<LV_4x4Matrix_t>;

        class LV_2x3Matrix_t
        {
        public:
            operator cv::Mat();
            cv::Mat as_cv_mat();

        private:
            double m_value[6];
        };

        using LV_2x3MatrixPtr_t = LV_Ptr_t<LV_2x3Matrix_t>;

#include "./reset_packing.hpp"
    }
}