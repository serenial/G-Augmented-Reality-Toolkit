#include <tuple>
#include <opencv2/opencv.hpp>
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

cv::Scalar lv_interop::LV_U32RGBColour_t::get_bgr()
{
    return cv::Scalar(blue, green, red);
}
cv::Scalar lv_interop::LV_U32RGBColour_t::get_bgra()
{
    return cv::Scalar(blue, green, red, alpha);
}
cv::Scalar lv_interop::LV_U32RGBColour_t::get_hsv()
{
    cv::Mat hsv;
    cv::cvtColor(cv::Mat(1, 1, CV_8UC3, get_bgr()), hsv, cv::COLOR_BGR2HSV);
    return cv::Scalar(hsv.data[0], hsv.data[1], hsv.data[2]);
}
cv::Scalar lv_interop::LV_U32RGBColour_t::get_blue()
{
    return cv::Scalar(blue);
}