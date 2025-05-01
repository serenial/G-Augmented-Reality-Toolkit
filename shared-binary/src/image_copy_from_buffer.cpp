#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_BufferInfo_t
    {
    public:
        operator cv::Mat() const;
        LV_BufferInfo_t() = delete;

    private:
        LV_1DArrayHandle_t<uint8_t> m_handle;
        uint16_t m_size_dim_0;
        int32_t m_step_dim_0;
        uint16_t m_size_dim_1;
        int32_t m_step_dim_1;
    };
#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_buffer(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_Ptr_t<LV_BufferInfo_t> info_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            lv_image dst(dst_edvr_ref_ptr);
            cv::Mat buffer = *info_ptr;

            buffer.copyTo(dst);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}

LV_BufferInfo_t::operator cv::Mat() const
{
    cv::MatStep step(m_step_dim_0);
    step[1] = m_step_dim_1;
    return cv::Mat(m_size_dim_0, m_size_dim_1, CV_8UC(step[1]), m_handle.begin(), step);
}