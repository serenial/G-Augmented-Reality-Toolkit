#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;
using namespace image;
using namespace std::rel_ops;

#include "g_ar_toolkit/lv-interop/set-packing.hpp"
using LV_RawMemoryPtrInfo_t = struct
{
    uint64_t ptr;
    uint16_t size_dim_0;
    int32_t step_dim_0;
    uint16_t size_dim_1;
    int32_t step_dim_1;
};

using LV_RawMemoryPtrInfoPtr_t = LV_Ptr_t<LV_RawMemoryPtrInfo_t>;

#include "g_ar_toolkit/lv-interop/reset-packing.hpp"

cv::Mat wrap_raw_memory_as_cv_mat(LV_RawMemoryPtrInfoPtr_t);

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_to_raw_memory(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_RawMemoryPtrInfoPtr_t dst_info_ptr)
    {
        try
        {
            image::Image src(src_edvr_ref_ptr);
            auto raw_memory =  wrap_raw_memory_as_cv_mat(dst_info_ptr);

            if(src.size().area() <= raw_memory.size().area()){
                src.copyTo(raw_memory);
            }
            else{
                (*src)(cv::Rect(cv::Point(0,0),raw_memory.size())).copyTo(raw_memory);
            }
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_raw_memory(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_RawMemoryPtrInfoPtr_t src_info_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            image::Image dst(dst_edvr_ref_ptr);
            auto raw_memory =  wrap_raw_memory_as_cv_mat(src_info_ptr);
            
            raw_memory.copyTo(dst);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }
}

cv::Mat wrap_raw_memory_as_cv_mat(LV_RawMemoryPtrInfoPtr_t info_ptr){

    cv::MatStep step(info_ptr->step_dim_0);
    step[1] = info_ptr->step_dim_1;
    cv::Mat wrapped(info_ptr->size_dim_0,info_ptr->size_dim_1, CV_8UC(step[1]), reinterpret_cast<void*>(info_ptr->ptr), step);

    return wrapped;

}
