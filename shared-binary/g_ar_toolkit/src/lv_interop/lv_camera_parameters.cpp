#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

cv::Mat LV_CameraDistortion_t::as_cv_mat()
{
    return cv::Mat{1, 14, CV_64F, &(m_value[0])};
}

cv::Mat LV_CameraDistortion_t::as_cv_mat(size_t length)
{
    return as_cv_mat().colRange(0,length);
}

LV_CameraDistortion_t::operator cv::Mat()
{
    return as_cv_mat();
}