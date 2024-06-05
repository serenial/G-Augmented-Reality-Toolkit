#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;
using namespace std::rel_ops;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"
using LV_RawMemoryPtrInfo_t = struct
{
    uint64_t ptr;
    uint16_t size_dim_0;
    int32_t step_dim_0;
    uint16_t size_dim_1;
    int32_t step_dim_1;
};

using LV_RawMemoryPtrInfoPtr_t = LV_Ptr_t<LV_RawMemoryPtrInfo_t>;

static cv::Mat wrap_raw_memory_as_cv_mat(LV_RawMemoryPtrInfoPtr_t);

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_to_raw_memory(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_RawMemoryPtrInfoPtr_t dst_info_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);
            auto raw_memory = wrap_raw_memory_as_cv_mat(dst_info_ptr);

            if (src.size().area() <= raw_memory.size().area())
            {
                src.copyTo(raw_memory);
            }
            else
            {
                (*src)(cv::Rect(cv::Point(0, 0), raw_memory.size())).copyTo(raw_memory);
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
            lv_image dst(dst_edvr_ref_ptr);
            auto raw_memory = wrap_raw_memory_as_cv_mat(src_info_ptr);

            raw_memory.copyTo(dst);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_map_and_get_raw_ptr(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t lock_lifetime_edvr_ref_ptr,
        LV_RawMemoryPtrInfoPtr_t src_info_ptr)
    {
        try
        {
            // create an EDVRManagedObject to manage the lifetime of the image lock
            // define a destructor that will user unlock the image
            class locked_image_lifetime_manager
            {
                const std::unique_ptr<lv_image> img_ptr;

            public:
                locked_image_lifetime_manager(LV_EDVRReferencePtr_t img_edvr_ref_ptr)
                    : img_ptr(std::make_unique<lv_image>(img_edvr_ref_ptr))
                {
                    img_ptr->upgrade_to_mapped();
                }
                ~locked_image_lifetime_manager()
                {
                    img_ptr->downgrade_from_mapped();
                }
                void get_memory_info(LV_RawMemoryPtrInfoPtr_t src_info_ptr)
                {
                    src_info_ptr->ptr = reinterpret_cast<uint64_t>(img_ptr->mat().data);
                    src_info_ptr->size_dim_0 = img_ptr->height();
                    src_info_ptr->size_dim_1 = img_ptr->width();
                    src_info_ptr->step_dim_0 = img_ptr->mat().step[0];
                    src_info_ptr->step_dim_1 = img_ptr->mat().step[1];
                }
            };

            locked_image_lifetime_manager *lifetime_mgr = new locked_image_lifetime_manager(src_edvr_ref_ptr);

            EDVRManagedObject<locked_image_lifetime_manager>(lock_lifetime_edvr_ref_ptr, lifetime_mgr);

            lifetime_mgr->get_memory_info(src_info_ptr);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}

static cv::Mat wrap_raw_memory_as_cv_mat(LV_RawMemoryPtrInfoPtr_t info_ptr)
{
    cv::MatStep step(info_ptr->step_dim_0);
    step[1] = info_ptr->step_dim_1;
    cv::Mat wrapped(info_ptr->size_dim_0, info_ptr->size_dim_1, CV_8UC(step[1]), reinterpret_cast<void *>(info_ptr->ptr), step);
    return wrapped;
}