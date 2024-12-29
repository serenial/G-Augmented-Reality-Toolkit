
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;
using namespace std::rel_ops;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_RawMemoryPtrInfo_t
    {
    public:
        operator cv::Mat() const;
        LV_RawMemoryPtrInfo_t &operator=(const lv_image &);
        LV_RawMemoryPtrInfo_t() = delete;

    private:
        uint64_t m_ptr;
        uint16_t m_size_dim_0;
        int32_t m_step_dim_0;
        uint16_t m_size_dim_1;
        int32_t m_step_dim_1;
    };
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    using LV_RawMemoryPtrInfoPtr_t = LV_Ptr_t<LV_RawMemoryPtrInfo_t>;

    // create an EDVRManagedObject to manage the lifetime of the image lock
    // define a destructor that will user unlock the image
    class Locked_Image_t
    {
    public:
        Locked_Image_t(LV_EDVRReferencePtr_t);
        ~Locked_Image_t();
        void copy_memory_info(LV_RawMemoryPtrInfoPtr_t) const;

    private:
        const std::unique_ptr<lv_image> m_img_ptr;
    };
}
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
            cv::Mat raw_memory = *dst_info_ptr;

            if (src.size().area() <= raw_memory.size().area())
            {
                src.copyTo(raw_memory);
            }
            else
            {
                src(cv::Rect(cv::Point(0, 0), raw_memory.size())).copyTo(raw_memory);
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            cv::Mat raw_memory = *src_info_ptr;

            raw_memory.copyTo(dst);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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

            EDVRManagedObject<Locked_Image_t> locked(lock_lifetime_edvr_ref_ptr, new Locked_Image_t(src_edvr_ref_ptr));

            locked->copy_memory_info(src_info_ptr);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}

LV_RawMemoryPtrInfo_t::operator cv::Mat() const
{
    cv::MatStep step(m_step_dim_0);
    step[1] = m_step_dim_1;
    return cv::Mat(m_size_dim_0, m_size_dim_1, CV_8UC(step[1]), reinterpret_cast<void *>(m_ptr), step);
}

LV_RawMemoryPtrInfo_t &LV_RawMemoryPtrInfo_t::operator=(const lv_image &img)
{
    m_ptr = reinterpret_cast<uint64_t>(img.mat().data);
    m_size_dim_0 = img.height();
    m_size_dim_1 = img.width();
    m_step_dim_0 = img.mat().step[0];
    m_step_dim_1 = img.mat().step[1];
    return *this;
}

Locked_Image_t::Locked_Image_t(LV_EDVRReferencePtr_t img_edvr_ref_ptr)
    : m_img_ptr(std::make_unique<lv_image>(img_edvr_ref_ptr))
{
    m_img_ptr->upgrade_to_mapped();
}
Locked_Image_t::~Locked_Image_t()
{
    m_img_ptr->downgrade_from_mapped();
}
void Locked_Image_t::copy_memory_info(LV_RawMemoryPtrInfoPtr_t src_info_ptr) const
{
    *src_info_ptr = *m_img_ptr;
}