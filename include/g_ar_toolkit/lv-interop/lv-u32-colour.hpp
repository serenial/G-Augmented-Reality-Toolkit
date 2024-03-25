#pragma once

#include <opencv2/core/types.hpp>
#include "./set-packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        struct LV_U32RGBColour_t
        {
            uint8_t blue, green, red, alpha;

            // define comparison methods
            friend bool operator==(const LV_U32RGBColour_t &l, const LV_U32RGBColour_t &r)
            {
                return std::tie(l.blue, l.green, l.red, l.alpha) == std::tie(r.blue, r.green, r.red, r.alpha);
            }
            friend bool operator!=(const LV_U32RGBColour_t &l, const LV_U32RGBColour_t &r)
            {
                return !(l == r);
            };

            cv::Scalar get_bgr();
            cv::Scalar get_bgra();
            cv::Scalar get_hsv();
            cv::Scalar get_blue();
        };
    }
}

#include "./reset-packing.hpp"