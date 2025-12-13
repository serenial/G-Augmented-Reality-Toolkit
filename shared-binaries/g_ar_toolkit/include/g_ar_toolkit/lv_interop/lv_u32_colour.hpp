#pragma once

#include <opencv2/core/types.hpp>

#include "./lv_image.hpp"

#include "./set_packing.hpp"

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

            LV_U32RGBColour_t& operator++();

            cv::Scalar get_bgr() const;
            cv::Scalar get_bgra() const;
            cv::Scalar get_hsv() const;
            cv::Scalar get_blue() const;

            cv::Scalar colour_for_image_type(const lv_image&) const;

            LV_U32RGBColour_t& adjust(float fractional_change);

            LV_U32RGBColour_t();
        };
    }
}

#include "./reset_packing.hpp"