#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"
#include "g_ar_toolkit/lv_interop/lv_termination_criteria.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

using LV_Chessboard_Points_t = struct
{
    uint8_t points_per_row, points_per_col;
};

using LV_Corner_t = struct
{
    float x, y;
};

using LV_Size_t = struct
{
    int32_t width, height;
};

using LV_CornerArrayHldPtr_t = LV_HandlePtr_t<LV_Array_t<1, LV_Corner_t>>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_find_chessboard_corners(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_Chessboard_Points_t *points_ptr,
        LV_Handle_t<LV_Array_t<1, uint8_t>> flags_array_handle,
        LV_CornerArrayHldPtr_t corners_handle_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            int flags = 0;

            // accumulate array of enums into flags
            for (size_t i = 0; i < array_handle_array_length(flags_array_handle); i++)
            {
                flags = flags | find_chessboard_corners_enum_to_flag((*flags_array_handle)->data[i]);
            }

            std::vector<cv::Point2f> corners;

            cv::Size pattern(points_ptr->points_per_row, points_ptr->points_per_col);

            cv::findChessboardCorners(src, pattern, corners, flags);

            copy_with_allocation_to_1d_lv_array_handle_ptr<std::vector<cv::Point2f>, LV_Corner_t>(corners, corners_handle_ptr, [](LV_Corner_t *out, bool _, cv::Point2f in)
                                                                                                  {
                out->x = in.x;
                out->y = in.y; });
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_corner_sub_pix(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_Size_t *window_size_ptr,
        LV_Size_t *zero_zone_ptr,
        LV_TerminationCritera_t *term_crit_ptr,
        LV_CornerArrayHldPtr_t corners_handle_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            std::vector<cv::Point2f> corners;

            for (size_t i = 0; i < array_handle_ptr_array_length(corners_handle_ptr); i++)
            {
                auto corner = (**corners_handle_ptr)->data[i];
                corners.emplace_back(corner.x, corner.y);
            }

            cv::cornerSubPix(src,
                             corners,
                             cv::Size(window_size_ptr->width, window_size_ptr->height),
                             cv::Size(zero_zone_ptr->width, zero_zone_ptr->height),
                             lv_termination_critera_ptr_to_cv_term_criteria(term_crit_ptr));

            copy_with_allocation_to_1d_lv_array_handle_ptr<std::vector<cv::Point2f>, LV_Corner_t>(corners, corners_handle_ptr, [](LV_Corner_t *out, bool _, cv::Point2f in)
                                                                                                  {
                out->x = in.x;
                out->y = in.y; });
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}