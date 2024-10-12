#include <stdexcept>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_array(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        void *array_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_ImageSizePtr_t array_size_ptr,
        uint8_t array_third_dim)
    {
        try
        {
            lv_image dst(dst_edvr_ref_ptr);
            // wrap raw pointer in Mat to make copy easy
            cv::Mat wrapped(*array_size_ptr, dst.cv_type(), array_ptr);

            switch (array_third_dim)
            {
            case 3:
                // 3D U8 Array with backwards pixel order
                dst.ensure_sized_to_match(*array_size_ptr);
                // swap channels into dst
                cv::cvtColor(wrapped, dst, cv::COLOR_BGR2BGRA);
                break;
            case 0:
            case 4:
                // 2D U8 or U32 array or 3D (4 byte) array
                // copy to dst
                wrapped.copyTo(dst);
                break;
            default:
                throw std::invalid_argument("Third dimension size parameter does not map to an expected value of 0, 3 or 4.");
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}