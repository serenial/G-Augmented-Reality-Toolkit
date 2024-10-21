#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

cv::Mat LV_3x3Matrix_t::as_cv_mat()
{
    return cv::Mat{3, 3, CV_64F, &(m_value[0])};
}

LV_3x3Matrix_t::operator cv::Mat()
{
    return as_cv_mat();
}