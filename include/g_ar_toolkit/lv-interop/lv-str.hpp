#pragma once

#include <string>

#include "./lv-types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        void lv_string_handle_to_string(LV_StringHandle_t, std::string&);
        std::string lv_string_handle_to_string(LV_StringHandle_t);
        LV_MgErr_t copy_std_string_to_lv_string_handle_ptr(std::string, LV_StringHandlePtr_t);
    }
}