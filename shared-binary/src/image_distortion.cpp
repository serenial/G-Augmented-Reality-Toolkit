#include <vector>
#include <numeric>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    // no packing modifications required - this object has no LabVIEW equivalent

    class ImageDistorter
    {
    public:
        ImageDistorter(const cv::Mat &camera_mat, const cv::Mat &distortion_coeffs, const cv::Size input_size);
        ImageDistorter() = delete;
        void distort(const cv::Mat &src, cv::Mat &dst, bool use_fixed_point_maps, int border_mode, cv::Scalar background) const;

    private:
        const cv::Size m_input_size;
        cv::Mat m_map_x_float, m_map_y_float, m_map_x_fixed, m_map_y_fixed;
    };
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_distortion_map_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ImageSizePtr_t size_ptr,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            EDVRManagedObject<ImageDistorter> image_rectifier(edvr_ref_ptr, new ImageDistorter(camera_matrix_ptr->as_cv_mat(), camera_distortion_ptr->as_cv_mat(5), *size_ptr));
        }
        catch (...)
        {
error_cluster_ptr.copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_distortion_map_distort_image(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t rectifier_edvr_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_BooleanPtr_t use_fixed,
        LV_EnumCVBoarderType_t border_mode,
        LV_U32RGBColour_t colour
    )
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            EDVRManagedObject<ImageDistorter> image_distorter(rectifier_edvr_ref_ptr);
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            image_distorter->distort(src, dst, *use_fixed, border_mode, colour.colour_for_image_type(dst));
        }
        catch (...)
        {
error_cluster_ptr.copy_from_exception(std::current_exception(),__func__);
        }
        return LV_ERR_noError;
    }
}

ImageDistorter::ImageDistorter(const cv::Mat &camera_mat, const cv::Mat &distortion_coeffs, const cv::Size input_size) : m_input_size(input_size),
                                                                                                                         m_map_x_float(cv::Mat(input_size, CV_32FC1)),
                                                                                                                         m_map_y_float(cv::Mat(input_size, CV_32FC1))
{
    cv::Mat camera_mat_undistorted = camera_mat.clone();

    std::vector<cv::Point2f> points_undistorted, points_distorted;

    points_undistorted.reserve(m_input_size.area());

    // fill points distorted with a list of x,y points
    for (int y = 0; y < m_input_size.height; ++y)
    {
        for (int x = 0; x < m_input_size.width; ++x)
        {
            points_distorted.emplace_back(float(x), float(y));
        }
    }

    cv::undistortPoints(points_distorted, points_undistorted, camera_mat, distortion_coeffs, cv::noArray(), camera_mat_undistorted);

    // copy vector of points into x and y maps
    for (int i = 0; i < points_undistorted.size(); i++)
    {
        const auto point = points_undistorted[i];
        auto [row, col] = std::div(i, m_input_size.width);
        m_map_x_float.at<float>(row, col) = point.x;
        m_map_y_float.at<float>(row, col) = point.y;
    }

    // init the faster fixed-point lookup maps
    cv::convertMaps(m_map_x_float, m_map_y_float, m_map_x_fixed, m_map_y_fixed, CV_16SC2, false);
}

void ImageDistorter::distort(const cv::Mat &src, cv::Mat &dst, bool use_fixed_point_maps,  int border_mode, cv::Scalar background) const
{

    const cv::Mat &map_x = use_fixed_point_maps ? m_map_x_fixed : m_map_x_float;
    const cv::Mat &map_y = use_fixed_point_maps ? m_map_y_fixed : m_map_y_float;

    cv::remap(src, dst, map_x, map_y, cv::INTER_LINEAR, border_mode, background);
}
