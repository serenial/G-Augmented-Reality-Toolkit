#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_camera_parameters.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    // no packing modifications required - this object has no LabVIEW equivalent
    class ImageRectifier
    {
    public:
        ImageRectifier(cv::Mat camera_mat, cv::Mat distortion_coeffs, cv::Size input_size, double alpha);
        ImageRectifier() = delete;
        void rectify(const cv::Mat &src, cv::Mat &dst, bool use_fixed_point_maps, bool crop_to_valid) const;
        void copy_new_camera_mat_to(cv::Mat) const;
        cv::Rect get_valid_pixel_rect() const;

    private:
        const cv::Size m_input_size;
        const cv::Rect m_valid_pixel_rect;
        cv::Mat m_new_camera_mat, m_map1_float, m_map2_float, m_map1_fixed, m_map2_fixed, m_buffer_greyscale, m_buffer_bgra;
    };
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_rectification_map_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_ImageSizePtr_t size_ptr,
        LV_CameraMatrixPtr_t camera_matrix_ptr,
        LV_CameraDistortionPtr_t camera_distortion_ptr,
        double alpha,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_ImagePointIntPtr_t roi_top_left_ptr,
        LV_ImageSizePtr_t roi_size_ptr)
    {
        try
        {
            EDVRManagedObject<ImageRectifier> image_rectifier(edvr_ref_ptr, new ImageRectifier(camera_matrix_ptr->as_cv_mat(), camera_distortion_ptr->as_cv_mat(), *size_ptr, alpha));

            image_rectifier->copy_new_camera_mat_to(camera_matrix_ptr->as_cv_mat());

            *roi_top_left_ptr = image_rectifier->get_valid_pixel_rect().tl();
            *roi_size_ptr = image_rectifier->get_valid_pixel_rect().size();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_rectification_map_rectify_image(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t rectifier_edvr_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_BooleanPtr_t crop_to_valid_pixels,
        LV_BooleanPtr_t use_fixed_point_maps)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            EDVRManagedObject<ImageRectifier> image_rectifier(rectifier_edvr_ref_ptr);
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            image_rectifier->rectify(src, dst, *use_fixed_point_maps, *crop_to_valid_pixels);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}

ImageRectifier::ImageRectifier(cv::Mat camera_mat, cv::Mat distortion_coeffs, cv::Size input_size, double alpha)
    : m_input_size(input_size),
      m_new_camera_mat(cv::getOptimalNewCameraMatrix(camera_mat, distortion_coeffs, input_size, alpha, cv::Size(), const_cast<cv::Rect *>(&m_valid_pixel_rect), false)),
      m_buffer_bgra(input_size, CV_8UC1),
      m_buffer_greyscale(input_size, CV_8UC4)
{
    // initialilze the undistored rectify map
    cv::initUndistortRectifyMap(camera_mat, distortion_coeffs, cv::Mat(), m_new_camera_mat, m_input_size, CV_32FC1, m_map1_float, m_map2_float);
    // init the faster fixed-point lookup maps
    cv::convertMaps(m_map1_float, m_map2_float, m_map1_fixed, m_map2_fixed, CV_16SC2, false);
}

void ImageRectifier::copy_new_camera_mat_to(cv::Mat dst) const
{
    m_new_camera_mat.copyTo(dst);
}

cv::Rect ImageRectifier::get_valid_pixel_rect() const
{
    return m_valid_pixel_rect;
}

void ImageRectifier::rectify(const cv::Mat &src, cv::Mat &dst, bool use_fixed_point_maps, bool crop_to_valid) const
{
    const cv::Mat &m1 = use_fixed_point_maps ? m_map1_fixed : m_map1_float;
    const cv::Mat &m2 = use_fixed_point_maps ? m_map2_fixed : m_map2_float;

    if (crop_to_valid)
    {
        // use internal image_rectifier buffer to accept result and then copy roi into dst
        if (dst.size() != m_valid_pixel_rect.size())
        {
            dst = cv::Mat(m_valid_pixel_rect.size(), src.type());
        }
        const cv::Mat &buffer = (dst.type() == m_buffer_bgra.type()) ? m_buffer_bgra : m_buffer_greyscale;
        cv::remap(src, buffer, m1, m2, cv::INTER_LINEAR);
        buffer(m_valid_pixel_rect).copyTo(dst);
    }
    else
    {
        if (dst.size() != m_input_size)
        {
            dst = cv::Mat(m_input_size, src.type());
        }
        cv::remap(src, dst, m1, m2, cv::INTER_LINEAR);
    }
}
