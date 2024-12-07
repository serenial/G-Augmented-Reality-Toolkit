#pragma once

#include <opencv2/core.hpp>

#include "./lv_types.hpp"
#include "./lv_mat.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
#include "./set_packing.hpp"



        using LV_CameraMatrix_t = LV_3x3Matrix_t;
        using LV_CameraMatrixPtr_t = LV_Ptr_t<LV_CameraMatrix_t>;

        class LV_CameraDistortion_t{
            public:
            operator cv::Mat();
            cv::Mat as_cv_mat();
            cv::Mat as_cv_mat(size_t length);
            private:
            double m_value [14];
        };

        using LV_CameraDistortionPtr_t = LV_Ptr_t<LV_CameraDistortion_t>;

#include "./reset_packing.hpp"
    }
}