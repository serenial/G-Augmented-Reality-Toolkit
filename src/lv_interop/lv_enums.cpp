#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

cv::BorderTypes border_type_enum_to_cv_border_type(uint8_t type)
{
    const cv::BorderTypes types[] = {
        cv::BorderTypes::BORDER_CONSTANT,
        cv::BorderTypes::BORDER_REPLICATE,
        cv::BorderTypes::BORDER_REFLECT,
        cv::BorderTypes::BORDER_WRAP,
        cv::BorderTypes::BORDER_TRANSPARENT,
        cv::BorderTypes::BORDER_DEFAULT,
        cv::BorderTypes::BORDER_ISOLATED};

    if (type < std::size(types))
    {
        return types[type];
    }

    throw std::out_of_range("The supplied value for the Border Type does not map to a valid OpenCV value.");
}

cv::InterpolationFlags interpolation_flag_enum_to_cv_interpolation_flag(uint8_t flag)
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

    if (flag < std::size(flags))
    {
        return flags[flag];
    }

    throw std::out_of_range("The supplied value for the Interpolation Flag does not map to a valid OpenCV value.");
}

cv::LineTypes line_types_enum_to_cv_linetype(uint8_t line_type)
{
    const cv::LineTypes line_types[] =
        {
            cv::FILLED,
            cv::LINE_4,
            cv::LINE_8,
            cv::LINE_AA};

    if (line_type < std::size(line_types))
    {
        return line_types[line_type];
    }

    throw std::out_of_range("The supplied value for the line-type does not map to a valid OpenCV value.");
}

cv::HersheyFonts font_face_enum_to_cv_hershey_font(uint8_t font)
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

    if (font < std::size(fonts))
    {
        return fonts[font];
    }

    throw std::out_of_range("The supplied value for the font-face does not map to a valid OpenCV value.");
}

int image_flip_enum_to_image_flip_code(uint8_t flip_value)
{
    const int flip_codes[] =
        {
            0, // flip-x
            1, // flip-y
            -1 // flip x and y
        };

    if (flip_value < std::size(flip_codes))
    {
        return flip_codes[flip_value];
    }

    throw std::out_of_range("The supplied value for the image flip type does not map to a valid OpenCV value.");
}

int find_chessboard_corners_enum_to_flag(uint8_t enum_value)
{

    const int flags[] = {
        cv::CALIB_CB_ADAPTIVE_THRESH,
        cv::CALIB_CB_NORMALIZE_IMAGE,
        cv::CALIB_CB_FILTER_QUADS,
        cv::CALIB_CB_FAST_CHECK
    };

    if (enum_value < std::size(flags))
    {
        return flags[enum_value];
    }

    throw std::out_of_range("The supplied value for the chessboard corner detection flag does not map to a valid OpenCV value.");
}