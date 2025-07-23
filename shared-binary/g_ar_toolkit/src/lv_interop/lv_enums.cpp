#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

LV_EnumCVBoarderType_t::operator cv::BorderTypes() const
{
    const cv::BorderTypes types[] = {
        cv::BorderTypes::BORDER_CONSTANT,
        cv::BorderTypes::BORDER_REPLICATE,
        cv::BorderTypes::BORDER_REFLECT,
        cv::BorderTypes::BORDER_WRAP,
        cv::BorderTypes::BORDER_TRANSPARENT,
        cv::BorderTypes::BORDER_DEFAULT,
        cv::BorderTypes::BORDER_ISOLATED};

    if (m_value < std::size(types))
    {
        return types[m_value];
    }

    throw std::out_of_range("The supplied value for the Border Type does not map to a valid OpenCV value.");
}

LV_EnumCVInterpolationFlag_t::operator cv::InterpolationFlags() const
{
    const cv::InterpolationFlags flags[] = {
        cv::InterpolationFlags::INTER_NEAREST,
        cv::InterpolationFlags::INTER_LINEAR,
        cv::InterpolationFlags::INTER_CUBIC,
        cv::InterpolationFlags::INTER_AREA,
        cv::InterpolationFlags::INTER_LANCZOS4,
        cv::InterpolationFlags::INTER_LINEAR_EXACT,
        cv::InterpolationFlags::INTER_NEAREST_EXACT,
        cv::InterpolationFlags::INTER_MAX};

    if (m_value < std::size(flags))
    {
        return flags[m_value];
    }

    throw std::out_of_range("The supplied value for the Interpolation Flag does not map to a valid OpenCV value.");
}