#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_AdaptiveThresholdMethods_t : public LV_EnumCVInt_t
    {
    public:
        // confusingly this enum is called AdaptiveThresholdTypes even through it is methods
        operator cv::AdaptiveThresholdTypes() const
        {
            const cv::AdaptiveThresholdTypes threshold_types[] =
                {
                    cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_MEAN_C,
                    cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_GAUSSIAN_C};

            if (m_value < std::size(threshold_types))
            {
                return threshold_types[m_value];
            }

            throw std::out_of_range("The supplied value for the threshold method type does not map to an OpenCV type.");
        }
    };

    class LV_ThresholdMethods_t : public LV_EnumCVInt_t
    {
    public:
        // confusingly this enum is called ThresholdTypes even through it is methods
        operator cv::ThresholdTypes() const
        {
            const cv::ThresholdTypes threshold_types[] =
                {
                    cv::ThresholdTypes::THRESH_BINARY,
                    cv::ThresholdTypes::THRESH_BINARY_INV,
                    cv::ThresholdTypes::THRESH_TRUNC,
                    cv::ThresholdTypes::THRESH_TOZERO,
                    cv::ThresholdTypes::THRESH_TOZERO_INV,
                    cv::ThresholdTypes::THRESH_MASK,
                    cv::ThresholdTypes::THRESH_OTSU,
                    cv::ThresholdTypes::THRESH_TRIANGLE};

            if (m_value < std::size(threshold_types))
            {
                return threshold_types[m_value];
            }

            throw std::out_of_range("The supplied value for the threshold type does not map to an OpenCV type.");
        }
    };
#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_adaptive_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        uint8_t max_value,
        LV_AdaptiveThresholdMethods_t adaptive_method,
        LV_BooleanPtr_t invert,
        LV_EnumBlockSize_t block_size,
        double c)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            if (src.is_bgra() || dst.is_bgra())
            {
                throw std::invalid_argument("Both source and destination should be Greyscale images.");
            }

            // destination will be auto-sized so no need to force with ensure_sized_to_match()
            cv::adaptiveThreshold(src, dst, max_value, adaptive_method, *invert ? cv::ThresholdTypes::THRESH_BINARY_INV : cv::ThresholdTypes::THRESH_BINARY, block_size, c);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_threshold(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        uint8_t max_value,
        uint8_t threshold,
        LV_1DArrayHandle_t<LV_ThresholdMethods_t> threshold_methods_array_handle)
    {
        try
        {
            throw_if_edvr_ref_pointers_not_unique({src_edvr_ref_ptr, dst_edvr_ref_ptr});
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            if (src.is_bgra() || dst.is_bgra())
            {
                throw std::invalid_argument("Both source and destination should be Greyscale images.");
            }

            // destination will be auto-sized so no need to force with ensure_sized_to_match()
            cv::threshold(src, dst, threshold, max_value, LV_EnumCVInt_t::combine(threshold_methods_array_handle));
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}