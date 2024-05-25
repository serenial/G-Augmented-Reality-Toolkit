#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"
#include "g_ar_toolkit/lv_interop/lv_functions.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

void lv_interop::lv_string_handle_to_string(LV_StringHandle_t handle, std::string &str)
{
    // get string size - caution handle may be null
    size_t string_length = handle && (*handle) && (*handle)->dims ? (*handle)->dims[0] : 0;
    // assign the handle value to the provided string
    str.assign((*handle)->data_ptr(), string_length);
}

std::string lv_interop::lv_string_handle_to_string(LV_StringHandle_t handle)
{
    // get string size - caution handle may be null
    size_t string_length = handle && (*handle) && (*handle)->dims ? (*handle)->dims[0] : 0;
    // assign the handle value to the provided string
    return std::string{(*handle)->data_ptr(), string_length};
}

LV_MgErr_t lv_interop::copy_std_string_to_lv_string_handle_ptr(std::string str, LV_StringHandlePtr_t handle_ptr)
{
    size_t str_start_len = (handle_ptr && *handle_ptr && **handle_ptr)? (**handle_ptr)->dims[0] : 0;

    ensure_array_handle_ptr_can_hold_n_elements(handle_ptr, str.length());
    
    std::memcpy((**handle_ptr)->data, str.c_str(), str.length());
    (**handle_ptr)->dims[0] = str.length();

    return LV_ERR_noError;
}
