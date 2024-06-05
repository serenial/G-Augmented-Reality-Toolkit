#ifndef G_AR_TK__INTEROP_LV_STR_HPP_
#define G_AR_TK__INTEROP_LV_STR_HPP_

#include <string>

#include "./lv_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        void lv_string_handle_to_string(LV_StringHandle_t, std::string&);
        std::string lv_string_handle_to_string(LV_StringHandle_t);
        LV_MgErr_t copy_std_string_to_lv_string_handle_ptr(const std::string&, LV_StringHandlePtr_t);
    }
}

#endif //G_AR_TK__INTEROP_LV_STR_HPP_