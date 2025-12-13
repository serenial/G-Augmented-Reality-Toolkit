#pragma once

#include <exception>
#include <stdexcept>
#include "./lv_types.hpp"
#include "./lv_str.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        // LabVIEW Error Cluster type
        class LV_ErrorClusterPtr_t
        {
        public:
            LV_ErrorClusterPtr_t() = delete;
            void copy_from_exception(std::exception_ptr ex, const char *caller_name);

        private:
#include "./set_packing.hpp"
            struct LV_Error_t
            {
                LV_Boolean_t status;
                LV_MgErr_t code;
                LV_StringHandle_t source;
            };
#include "./reset_packing.hpp"
            LV_Ptr_t<LV_Error_t> m_err;
        };
    }
}