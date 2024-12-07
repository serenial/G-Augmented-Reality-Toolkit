#pragma once

#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
#include "./set_packing.hpp"

        class LV_3x3Matrix_t
        {
        public:
            operator cv::Mat();
            cv::Mat as_cv_mat();

        private:
            double m_value[9];
        };

        using LV_3x3MatrixPtr_t = LV_Ptr_t<LV_3x3Matrix_t>;

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