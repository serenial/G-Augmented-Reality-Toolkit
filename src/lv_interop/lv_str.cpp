#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"
#include "g_ar_toolkit/lv_interop/lv_functions.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

std::string_view lv_interop::lv_string_handle_to_std_string_view(LV_StringHandle_t handle)
{
    // get string size - caution handle may be null
    size_t string_length = handle && (*handle) && (*handle)->dims ? (*handle)->dims[0] : 0;

    return std::string_view{(*handle)->data_ptr(), string_length};
}


std::string lv_interop::lv_string_handle_to_std_string(LV_StringHandle_t handle){
    return std::string(lv_string_handle_to_std_string_view(handle));
}

LV_MgErr_t lv_interop::copy_std_string_view_to_lv_string_handle_ptr(std::string_view str, LV_StringHandlePtr_t handle_ptr)
{
    size_t str_start_len = (handle_ptr && *handle_ptr && **handle_ptr)? (**handle_ptr)->dims[0] : 0;

    ensure_array_handle_ptr_can_hold_n_elements(handle_ptr, str.length());
    
    std::memcpy((**handle_ptr)->data, str.data(), str.size());
    (**handle_ptr)->dims[0] = str.size();

    return LV_ERR_noError;
}
