#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    using LV_ShapePointsArrayHandle_t = LV_1DArrayHandle_t<LV_ImagePointInt_t>;
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_shape_approx_poly_douglas_peucker(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ShapePointsArrayHandle_t points_array_handle,
        double epsilon,
        LV_BooleanPtr_t closed)
    {
        try
        {
            std::vector<cv::Point2i> curve;
            cv::approxPolyDP(points_array_handle.to_vector<cv::Point2i>(), curve, epsilon, *closed);
            // copy result back into handle
            points_array_handle.copy_element_by_element_from(curve);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_shape_approx_poly_n(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ShapePointsArrayHandle_t points_array_handle,
        uint8_t n_sides,
        double epsilon_pct,
        LV_BooleanPtr_t ensure_convex)
    {
        try
        {
            std::vector<cv::Point2i> curve;
            cv::approxPolyN(points_array_handle.to_vector<cv::Point2i>(), curve, n_sides, epsilon_pct, *ensure_convex);
            // copy result back into handle
            points_array_handle.copy_element_by_element_from(curve);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_shape_contours_arc_length(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ShapePointsArrayHandle_t points_array_handle,
        double *arc_length,
        LV_BooleanPtr_t closed)
    {
        try
        {
            *arc_length = cv::arcLength(points_array_handle.to_vector<cv::Point2i>(), *closed);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_shape_contour_area(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ShapePointsArrayHandle_t points_array_handle,
        double *area,
        LV_BooleanPtr_t oriented)
    {
        try
        {
            *area = cv::contourArea(points_array_handle.to_vector<cv::Point2i>(), *oriented);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_shape_convex_hull(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ShapePointsArrayHandle_t points_array_handle,
        LV_BooleanPtr_t clockwise)
    {
        try
        {
            std::vector<cv::Point2i> hull;
            cv::convexHull(points_array_handle.to_vector<cv::Point2i>(), hull, *clockwise, true);

            points_array_handle.copy_element_by_element_from(hull);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}