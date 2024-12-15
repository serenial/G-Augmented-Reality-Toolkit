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

namespace
{
    #include "g_ar_toolkit/lv_interop/set_packing.hpp"
    struct LV_EnumCVChessboardCorners_t : public LV_EnumCVInt_t
    {
        operator int() const
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
    };

    struct LV_EnumCVChessboardCornersSB_t : public LV_EnumCVInt_t
    {
        operator int() const
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
    };
    #include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

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

            corners_handle.copy_element_by_element_from(corners);
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

            corners_handle.copy_element_by_element_from(corners);
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

            corners_handle.copy_element_by_element_from(corners);

            meta_handle_ptr.copy_memory_from(meta);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}