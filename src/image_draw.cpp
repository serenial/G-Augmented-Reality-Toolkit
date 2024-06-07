#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

using LV_PointI32_t = struct
{
    int32_t x, y;
};

using LV_SizeU16_t = struct
{
    uint16_t width, height;
};

using LV_PointI32Ptr_t = LV_Ptr_t<LV_PointI32_t>;
using LV_SizeU16Ptr_t = LV_Ptr_t<LV_SizeU16_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_line(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_PointI32Ptr_t point_a_ptr,
        LV_PointI32Ptr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::line(
                (*img),
                cv::Point{point_a_ptr->x, point_a_ptr->y},
                cv::Point{point_b_ptr->x, point_b_ptr->y},
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_arrowed_line(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_PointI32Ptr_t point_a_ptr,
        LV_PointI32Ptr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type,
        double tip_length)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::arrowedLine(
                (*img),
                cv::Point{point_a_ptr->x, point_a_ptr->y},
                cv::Point{point_b_ptr->x, point_b_ptr->y},
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type),
                0,
                tip_length);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_circle(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_PointI32Ptr_t centre_ptr,
        uint16_t radius,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::circle(
                (*img),
                cv::Point{centre_ptr->x, centre_ptr->y},
                radius,
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_ellipse(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_PointI32Ptr_t centre_ptr,
        LV_SizeU16Ptr_t axes_ptr,
        double angle,
        double start_angle,
        double end_angle,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::ellipse(
                (*img),
                cv::Point{centre_ptr->x, centre_ptr->y},
                cv::Size{axes_ptr->width, axes_ptr->height},
                angle,
                start_angle,
                end_angle,
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_rectangle(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_PointI32Ptr_t point_a_ptr,
        LV_PointI32Ptr_t point_b_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::rectangle(
                (*img),
                cv::Point{point_a_ptr->x, point_a_ptr->y},
                cv::Point{point_b_ptr->x, point_b_ptr->y},
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_polylines(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_Handle_t<LV_Array_t<1, LV_PointI32_t>> points_array_handle,
        LV_BooleanPtr_t is_closed_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            std::vector<cv::Point> points;

            copy_1d_lv_array_handle_to_vector<LV_PointI32_t, cv::Point>(
                points_array_handle,
                points,
                [](LV_PointI32_t point)
                { return cv::Point{point.x, point.y}; });

            cv::polylines(
                (*img),
                points,
                *is_closed_ptr,
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_fillpoly(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_Handle_t<LV_Array_t<1, LV_PointI32_t>> points_array_handle,
        LV_BooleanPtr_t is_convex_ptr,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            std::vector<cv::Point> points;

            copy_1d_lv_array_handle_to_vector<LV_PointI32_t, cv::Point>(
                points_array_handle,
                points,
                [](LV_PointI32_t point)
                { return cv::Point{point.x, point.y}; });

            if (*is_convex_ptr)
            {
                cv::fillConvexPoly(
                    (*img),
                    points,
                    img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                    line_types_enum_to_cv_linetype(line_type));
            }
            else
            {
                cv::fillPoly(
                    (*img),
                    points,
                    img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                    line_types_enum_to_cv_linetype(line_type));
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_text(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t img_edvr_ref_ptr,
        LV_StringHandle_t text_handle,
        LV_PointI32Ptr_t origin_ptr,
        uint8_t font_face,
        double font_scale,
        LV_U32RGBColour_t colour,
        int32_t thickness,
        uint8_t line_type,
        LV_BooleanPtr_t bottom_left_org_ptr)
    {
        try
        {
            lv_image img(img_edvr_ref_ptr);

            cv::putText(
                (*img),
                lv_string_handle_to_std_string(text_handle),
                cv::Point{origin_ptr->x, origin_ptr->y},
                font_face_enum_to_cv_hershey_font(font_face),
                font_scale,
                img.is_bgra() ? colour.get_bgra() : colour.get_blue(),
                thickness,
                line_types_enum_to_cv_linetype(line_type),
                *bottom_left_org_ptr);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_get_text_size(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t text_handle,
        uint8_t font_face,
        double font_scale,
        int32_t thickness,
        int32_t *base_line_ptr,
        LV_SizeU16Ptr_t size_ptr)
    {
        try
        {
            cv::Size size = cv::getTextSize(
                lv_string_handle_to_std_string(text_handle),
                font_face_enum_to_cv_hershey_font(font_face),
                font_scale,
                thickness,
                base_line_ptr);

            size_ptr->height = size.height;
            size_ptr->width = size.width;
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_get_text_font_scale_from_height(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        uint8_t font_face,
        int32_t thickness,
        uint16_t pixel_height,
        double *font_scale)
    {
        try
        {
            *font_scale = cv::getFontScaleFromHeight(
                font_face_enum_to_cv_hershey_font(font_face),
                pixel_height,
                thickness);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}