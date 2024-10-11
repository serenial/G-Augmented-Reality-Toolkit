#ifndef G_AR_TK__INTEROP_LV_CAMERA_PARAMETERS_HPP_
#define G_AR_TK__INTEROP_LV_CAMERA_PARAMETERS_HPP_

#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
#include "./set_packing.hpp"

        class LV_CameraMatrix_t{
            public:
            operator cv::Mat();
            cv::Mat as_cv_mat();
            private:
            double m_value [9];
        };

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

#endif // G_AR_TK__INTEROP_LV_CAMERA_PARAMETERS_HPP_