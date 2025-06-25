#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_EnumCVLineType_t
    {
    private:
        uint8_t m_value;

    public:
        operator cv::LineTypes() const
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
    };

    class LV_EnumCVHersheyFont_t
    {
    private:
        uint8_t m_value;

    public:
        operator cv::HersheyFonts() const
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
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_line(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_ImagePointIntPtr_t point_a_ptr,
        LV_ImagePointIntPtr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::line(
                (*img),
                *point_a_ptr,
                *point_b_ptr,
                colour.colour_for_image_type(img),
                thickness,
                line_type);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_arrowed_line(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_ImagePointIntPtr_t point_a_ptr,
        LV_ImagePointIntPtr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type,
        double tip_length)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::arrowedLine(
                (*img),
                *point_a_ptr,
                *point_b_ptr,
                colour.colour_for_image_type(img),
                thickness,
                line_type,
                0,
                tip_length);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_circle(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_ImagePointIntPtr_t centre_ptr,
        uint16_t radius,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::circle(
                (*img),
                *centre_ptr,
                radius,
                colour.colour_for_image_type(img),
                thickness,
                line_type);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_ellipse(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_ImagePointIntPtr_t centre_ptr,
        LV_ImageSizePtr_t axes_ptr,
        double angle,
        double start_angle,
        double end_angle,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::ellipse(
                (*img),
                *centre_ptr,
                *axes_ptr,
                angle,
                start_angle,
                end_angle,
                colour.colour_for_image_type(img),
                thickness,
                line_type);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_rectangle(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_ImagePointIntPtr_t point_a_ptr,
        LV_ImagePointIntPtr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::rectangle(
                (*img),
                *point_a_ptr,
                *point_b_ptr,
                colour.colour_for_image_type(img),
                thickness,
                line_type);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_polylines(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_ImagePointInt_t> points_array_handle,
        LV_BooleanPtr_t is_closed_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::polylines(
                (*img),
                points_array_handle.to_vector<cv::Point2i>(),
                *is_closed_ptr,
                colour.colour_for_image_type(img),
                thickness,
                line_type);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_fillpoly(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_ImagePointInt_t> points_array_handle,
        LV_BooleanPtr_t is_convex_ptr,
        LV_U32RGBColour_t colour,
        LV_EnumCVLineType_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            if (*is_convex_ptr)
            {
                cv::fillConvexPoly(
                    (*img),
                    points_array_handle.to_vector<cv::Point2i>(),
                    colour.colour_for_image_type(img),
                    line_type);
            }
            else
            {
                cv::fillPoly(
                    (*img),
                    points_array_handle.to_vector<cv::Point2i>(),
                    colour.colour_for_image_type(img),
                    line_type);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_text(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_StringHandle_t text_handle,
        LV_ImagePointIntPtr_t origin_ptr,
        LV_EnumCVHersheyFont_t font_face,
        double font_scale,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        LV_EnumCVLineType_t line_type,
        LV_BooleanPtr_t bottom_left_org_ptr)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::putText(
                (*img),
                text_handle,
                *origin_ptr,
                font_face,
                font_scale,
                colour.colour_for_image_type(img),
                thickness,
                line_type,
                *bottom_left_org_ptr);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_get_text_size(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t text_handle,
        LV_EnumCVHersheyFont_t font_face,
        double font_scale,
        int32_t thickness,
        int32_t *base_line_ptr,
        LV_ImageSizePtr_t size_ptr)
    {
        try
        {
            cv::Size size = cv::getTextSize(
                text_handle,
                font_face,
                font_scale,
                thickness,
                base_line_ptr);

            *size_ptr = size;
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_get_text_font_scale_from_height(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EnumCVHersheyFont_t font_face,
        int32_t thickness,
        uint16_t pixel_height,
        double *font_scale)
    {
        try
        {
            *font_scale = cv::getFontScaleFromHeight(
                font_face,
                pixel_height,
                thickness);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}