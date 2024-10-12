#include <functional>
#include <algorithm>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>


#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_termination_criteria.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_find_chessboard_corners(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_ImageSizePtr_t pattern_ptr,
        LV_1DArrayHandle_t<LV_EnumCVChessboardCorners_t> flags_array_handle,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> corners_handle)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            // cv::Point2f cannot point to LabVIEW managed memory so we have to copy from corners_handle
            // into a std::vector and then copy that vector back into the corners_handle after the function call

            std::vector<cv::Point2f> corners;

            cv::findChessboardCorners(src, *pattern_ptr, corners, LV_EnumCVInt_t::combine(flags_array_handle));

            corners_handle.copy_from(corners, [](auto from, auto to){
                *to = from;
            });
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_refine_corner_sub_pix(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_WindowSizePtr_t window_size_ptr,
        LV_WindowSizePtr_t zero_zone_ptr,
        LV_Ptr_t<LV_TerminationCritera_t> term_crit_ptr,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> corners_handle)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            // cv::Point2f cannot point to LabVIEW managed memory so we have to copy from corners_handle
            // into a std::vector and then copy that vector back into the corners_handle after the function call

            std::vector<cv::Point2f> corners = corners_handle.to_vector<cv::Point2f>();

            cv::cornerSubPix(src, corners, *window_size_ptr, *zero_zone_ptr, *term_crit_ptr);

            corners_handle.copy_from(corners, [](auto from, auto to){
                *to = from;
            });
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_find_chessboard_corners_sb(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_ImageSizePtr_t pattern_ptr,
        LV_1DArrayHandle_t<LV_EnumCVChessboardCornersSB_t> flags_array_handle,
        LV_1DArrayHandle_t<LV_ImagePointFloat_t> corners_handle,
        LV_2DArrayHandle_t<uint8_t> meta_handle_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            cv::Mat meta{*pattern_ptr, CV_8UC1};

            // cv::Point2f cannot point to LabVIEW managed memory so we have to copy from corners_handle
            // into a std::vector and then copy that vector back into the corners_handle after the function call

            std::vector<cv::Point2f> corners;

            cv::findChessboardCornersSB(src, *pattern_ptr, corners, LV_EnumCVInt_t::combine(flags_array_handle), meta);

            corners_handle.copy_from(corners, [](auto from, auto to){
                *to = from;
            });

            meta_handle_ptr.copy_from(meta);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}