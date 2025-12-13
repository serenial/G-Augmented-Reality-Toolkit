#include <cstdlib>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    struct LV_BufferInfo_t
    {
        uint16_t m_size_dim_0; // number of pixels in the image row
        int32_t m_step_dim_0; // number of bytes to increment to advance to the next row
        uint16_t m_size_dim_1; // number of pixels per col
        int32_t m_step_dim_1;  // number of bytes to increment to get to the next col
    };
#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_buffer(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<uint8_t> buffer_handle,
        LV_Ptr_t<LV_BufferInfo_t> info_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr)
    {
        try
        {
            if(info_ptr->m_step_dim_0 == 0){
                throw std::invalid_argument("Dimension-zero step cannot be 0.");
            }

            lv_image dst(dst_edvr_ref_ptr);
            
            // determine max image size based on handle size
            auto buffer_max_height = std::div(static_cast<int32_t>(buffer_handle.size()), info_ptr->m_step_dim_0);

            auto height = std::min(std::abs(buffer_max_height.quot), static_cast<int32_t>(info_ptr->m_size_dim_0));

            // create cv::Mat representing buffer and copyTo destination
            cv::MatStep step(info_ptr->m_step_dim_0);
            step[1] = info_ptr->m_step_dim_1;

            cv::Mat(height, info_ptr->m_size_dim_1, CV_8UC(info_ptr->m_step_dim_1), info_ptr->m_size_dim_0 > 0 ? buffer_handle.begin() : buffer_handle.end()-1, step).copyTo(dst);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}