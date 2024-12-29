#include <tuple>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"

#include "g_ar_toolkit_export.h"

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
    cv::cvtColor(cv::Mat(1, 1, CV_8UC3, get_bgr()), hsv, cv::COLOR_BGR2HSV_FULL);
    return cv::Scalar(hsv.data[0], hsv.data[1], hsv.data[2]);
}
cv::Scalar LV_U32RGBColour_t::get_blue() const
{
    return cv::Scalar(blue);
}

cv::Scalar LV_U32RGBColour_t::colour_for_image_type(const lv_image &img) const
{
    return img.is_bgra() ? get_bgra() : get_blue();
}

LV_U32RGBColour_t &LV_U32RGBColour_t::operator++()
{
    if (blue < 255)
    {
        blue++;
    }
    if (red < 255)
    {
        red++;
    }
    if (green < 255)
    {
        green++;
    }
    return *this;
}

LV_U32RGBColour_t &LV_U32RGBColour_t::adjust(float fractional_change)
{
    auto hsv = get_hsv();
    for(int c = 0; c<3; c++){
        hsv[c] = std::round(std::clamp((1 + fractional_change) * float(hsv[c]), 0.0f, 255.0f));
    }

    cv::Mat bgr;
    cv::cvtColor(cv::Mat(1, 1, CV_8UC3, hsv), bgr, cv::COLOR_HSV2BGR_FULL);
    blue = bgr.data[0];
    green = bgr.data[1];
    red = bgr.data[2];

    return *this;
}

LV_U32RGBColour_t::LV_U32RGBColour_t() : alpha(255), red(0), blue(0), green(0) {};

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_compute_colour_pair(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        float fractional_change_lower,
        float fractional_change_upper,
        LV_U32RGBColour_t *lower,
        LV_U32RGBColour_t *upper)
    {
        try
        {
            lower->adjust(-fractional_change_lower);
            upper->adjust(fractional_change_upper);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}