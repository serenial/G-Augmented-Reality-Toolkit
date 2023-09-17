#include "g_ar_toolkit/lv-interop/lv-str.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

void lv_interop::lv_string_handle_to_string(LV_StringHandle_t handle, std::string& str){
    // get string size - caution handle may be null
    size_t string_length = handle && (*handle) && (*handle)->length ? (*handle)->length : 0;
    // assign the handle value to the provided string
    str.assign(reinterpret_cast<char *>((*handle)->str), string_length);
}

std::string lv_interop::lv_string_handle_to_string(LV_StringHandle_t handle){
    // get string size - caution handle may be null
    size_t string_length = handle && (*handle) && (*handle)->length ? (*handle)->length : 0;
    // assign the handle value to the provided string
    return std::string{reinterpret_cast<char *>((*handle)->str), string_length};
}

