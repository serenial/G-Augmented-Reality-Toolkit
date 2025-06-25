#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_MorphologyShapes_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::MorphShapes() const
        {
            const cv::MorphShapes morph_shapes[] =
                {
                    cv::MorphShapes::MORPH_RECT,
                    cv::MorphShapes::MORPH_CROSS,
                    cv::MorphShapes::MORPH_ELLIPSE};

            if (m_value < std::size(morph_shapes))
            {
                return morph_shapes[m_value];
            }

            throw std::out_of_range("The supplied value for the morph-shape does not map to an OpenCV type.");
        }
    };

    class LV_MorphologyTypes_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::MorphTypes() const
        {
            const cv::MorphTypes morph_types[] =
                {
                    cv::MorphTypes::MORPH_ERODE,
                    cv::MorphTypes::MORPH_DILATE,
                    cv::MorphTypes::MORPH_OPEN,
                    cv::MorphTypes::MORPH_CLOSE,
                    cv::MorphTypes::MORPH_GRADIENT,
                    cv::MorphTypes::MORPH_TOPHAT,
                    cv::MorphTypes::MORPH_BLACKHAT,
                    cv::MorphTypes::MORPH_HITMISS};

            if (m_value < std::size(morph_types))
            {
                return morph_types[m_value];
            }

            throw std::out_of_range("The supplied value for the morph-type does not map to an OpenCV type.");
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_morphology(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_MorphologyTypes_t operation,
        LV_2DArrayHandle_t<LV_Boolean_t> kernel_handle,
        LV_ImagePointIntPtr_t anchor_ptr,
        uint16_t iterations)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            cv::morphologyEx(src, src,
                             operation,
                             kernel_handle.as_cv_mat(CV_8UC1), // abuse fact that LV_Boolean_t is just a uint8_t
                             *anchor_ptr,
                             iterations);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_morph_structuring_element(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_MorphologyShapes_t shape_type,
        LV_ImageSizePtr_t size_ptr,
        LV_ImagePointIntPtr_t anchor_ptr,
        LV_2DArrayHandle_t<LV_Boolean_t> structuring_element_handle)
    {
        try
        {
            cv::Mat sem = cv::getStructuringElement(shape_type, *size_ptr, *anchor_ptr);

            structuring_element_handle.size_to_fit({sem.rows, sem.cols});

            // abuse fact that LV_Boolean_t is just a uint8_t
            sem.copyTo(structuring_element_handle.as_cv_mat(CV_8UC1));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}