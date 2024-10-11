#include <tuple>
#include <opencv2/opencv.hpp>
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

cv::Scalar LV_U32RGBColour_t::get_bgr() const
{
    return cv::Scalar(blue, green, red);
}
cv::Scalar LV_U32RGBColour_t::get_bgra() const
{
    return cv::Scalar(blue, green, red, alpha);
}
cv::Scalar LV_U32RGBColour_t::get_hsv() const
{
    cv::Mat hsv;
    cv::cvtColor(cv::Mat(1, 1, CV_8UC3, get_bgr()), hsv, cv::COLOR_BGR2HSV);
    return cv::Scalar(hsv.data[0], hsv.data[1], hsv.data[2]);
}
cv::Scalar LV_U32RGBColour_t::get_blue() const
{
    return cv::Scalar(blue);
}

cv::Scalar LV_U32RGBColour_t::colour_for_image_type(const lv_image& img) const{
    return img.is_bgra() ? get_bgra() : get_blue();
}