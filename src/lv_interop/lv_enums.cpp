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

LV_EnumCVLineType_t::operator cv::LineTypes() const
{
    const cv::LineTypes line_types[] =
        {
            cv::FILLED,
            cv::LINE_4,
            cv::LINE_8,
            cv::LINE_AA};

    if (m_value < std::size(line_types))
    {
        return line_types[m_value];
    }

    throw std::out_of_range("The supplied value for the line-type does not map to a valid OpenCV value.");
}

LV_EnumCVHersheyFont_t::operator cv::HersheyFonts() const
{
    const cv::HersheyFonts fonts[] =
        {
            cv::FONT_HERSHEY_SIMPLEX,
            cv::FONT_HERSHEY_PLAIN,
            cv::FONT_HERSHEY_DUPLEX,
            cv::FONT_HERSHEY_COMPLEX,
            cv::FONT_HERSHEY_TRIPLEX,
            cv::FONT_HERSHEY_COMPLEX_SMALL,
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
            cv::FONT_HERSHEY_SCRIPT_COMPLEX,
            cv::FONT_ITALIC};

    if (m_value < std::size(fonts))
    {
        return fonts[m_value];
    }

    throw std::out_of_range("The supplied value for the font-face does not map to a valid OpenCV value.");
}

LV_EnumCVImageFlip_t::operator int() const
{
    const int flip_codes[] =
        {
            0, // flip-x
            1, // flip-y
            -1 // flip x and y
        };

    if (m_value < std::size(flip_codes))
    {
        return flip_codes[m_value];
    }

    throw std::out_of_range("The supplied value for the image flip type does not map to a valid OpenCV value.");
}

LV_EnumCVChessboardCorners_t::operator int() const
{

    const int flags[] = {
        cv::CALIB_CB_ADAPTIVE_THRESH,
        cv::CALIB_CB_NORMALIZE_IMAGE,
        cv::CALIB_CB_FILTER_QUADS,
        cv::CALIB_CB_FAST_CHECK};

    if (m_value < std::size(flags))
    {
        return flags[m_value];
    }

    throw std::out_of_range("The supplied value for the chessboard corner detection flag does not map to a valid OpenCV value.");
}

LV_EnumCVChessboardCornersSB_t::operator int() const
{
    const int flags[] = {
        cv::CALIB_CB_NORMALIZE_IMAGE,
        cv::CALIB_CB_EXHAUSTIVE,
        cv::CALIB_CB_ACCURACY,
        cv::CALIB_CB_LARGER,
        cv::CALIB_CB_MARKER};

    if (m_value < std::size(flags))
    {
        return flags[m_value];
    }

    throw std::out_of_range("The supplied value for the chessboard corner detection flag does not map to a valid OpenCV value.");
}

LV_EnumCVCameraCalibrationFlags_t::operator int() const
{
    const int flags[] = {
        cv::CALIB_USE_INTRINSIC_GUESS,
        cv::CALIB_FIX_PRINCIPAL_POINT,
        cv::CALIB_FIX_ASPECT_RATIO,
        cv::CALIB_ZERO_TANGENT_DIST,
        cv::CALIB_FIX_FOCAL_LENGTH,
        cv::CALIB_FIX_K1,
        cv::CALIB_RATIONAL_MODEL,
        cv::CALIB_THIN_PRISM_MODEL,
        cv::CALIB_FIX_S1_S2_S3_S4,
        cv::CALIB_TILTED_MODEL,
        cv::CALIB_FIX_TAUX_TAUY};

    if (m_value < std::size(flags))
    {
        return flags[m_value];
    }

    throw std::out_of_range("The supplied value for the chessboard corner detection flag does not map to a valid OpenCV value.");
}