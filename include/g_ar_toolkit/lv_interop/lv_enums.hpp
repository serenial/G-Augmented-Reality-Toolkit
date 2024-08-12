#ifndef G_AR_TK__INTEROP_LV_ENUMS_HPP_
#define G_AR_TK__INTEROP_LV_ENUMS_HPP_

#include <opencv2/core/base.hpp>
#include <opencv2/imgproc.hpp>

cv::BorderTypes border_type_enum_to_cv_border_type(uint8_t);

cv::InterpolationFlags interpolation_flag_enum_to_cv_interpolation_flag(uint8_t);

cv::LineTypes line_types_enum_to_cv_linetype(uint8_t);

cv::HersheyFonts font_face_enum_to_cv_hershey_font(uint8_t);

int image_flip_enum_to_image_flip_code(uint8_t);

int find_chessboard_corners_enum_to_flag(uint8_t);

#endif //G_AR_TK__INTEROP_LV_ARRAY_HPP_