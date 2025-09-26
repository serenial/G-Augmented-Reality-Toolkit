#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_CLAHE_t
    {
        cv::Ptr<cv::CLAHE> clahe;

    public:
        LV_CLAHE_t(double clip_limit, cv::Size grid_size) : clahe(cv::createCLAHE(clip_limit, grid_size)) {}
        void apply_conversion(cv::Mat &src, cv::Mat &dst, double scaling_factor)
        {

            cv::Mat intermediate(src.size(), CV_16UC1);

            // apply transform
            clahe->apply(src, intermediate);
            
            // convert to U8
            intermediate.convertTo(dst, CV_8UC1, scaling_factor);
        }
        void apply_conversion(cv::Mat &src, cv::Mat  &dst)
        {
            // apply transform
            clahe->apply(src, dst);
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_clahe_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        double clip_limit,
        LV_ImageSizePtr_t title_grid_size)
    {
        try
        {
            EDVRManagedObject<LV_CLAHE_t>(edvr_ref_ptr, new LV_CLAHE_t(clip_limit, *title_grid_size));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_clahe_apply_to_mono_16_array(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t clahe_ref_ptr,
        LV_2DArrayHandle_t<uint16_t> data_handle,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        double scaling_factor
    )
    {
        try
        {
            cv::Mat src = data_handle.as_cv_mat(CV_16UC1);

            lv_image dst(dst_edvr_ref_ptr);
            dst.ensure_sized_to_match(src.size());

            EDVRManagedObject<LV_CLAHE_t>(clahe_ref_ptr)->apply_conversion(src, dst, scaling_factor);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_clahe_apply_to_edvr_buffer_mono_16(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t clahe_ref_ptr,
        LV_EDVRReferencePtr_t data_ref_ptr,
        LV_ImageSizePtr_t size_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        double scaling_factor
    )
    {

        LV_EDVRContext_t ctx = 0;

        try
        {

            if (EDVR_GetCurrentContext(&ctx))
            {
                throw std::runtime_error("Failed to obtain the application-context.");
            }

            LV_EDVRDataPtr_t data_ptr = nullptr;

            if (EDVR_AddRefWithContext(*data_ref_ptr, ctx, &data_ptr))
            {
                ctx = 0; // clear the ctx to indicate that clean-up isn't required
                throw std::runtime_error("Unable to dereference the supplied EDVR Refnum to valid data in this application-context.");
            }

            cv::Mat src(*size_ptr, CV_16UC1, data_ptr->sub_array.data_ptr);

            lv_image dst(dst_edvr_ref_ptr);
            dst.ensure_sized_to_match(src.size());

            EDVRManagedObject<LV_CLAHE_t>(clahe_ref_ptr)->apply_conversion(src, dst, scaling_factor);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        if (ctx)
        {
            EDVR_ReleaseRefWithContext(*data_ref_ptr, ctx);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_clahe_apply_to_image(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t clahe_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);
            lv_image dst(dst_edvr_ref_ptr);

            EDVRManagedObject<LV_CLAHE_t>(clahe_ref_ptr)->apply_conversion(src, dst);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}