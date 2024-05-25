#ifndef G_AR_TK__INTEROP_LV_ERROR_HPP_
#define G_AR_TK__INTEROP_LV_ERROR_HPP_

#include <exception>
#include <stdexcept>
#include "./lv_types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        LV_MgErr_t write_to_lv_error_cluster_ptr(LV_ErrorClusterPtr_t, LV_MgErr_t, const char* ,const char*);
        LV_MgErr_t write_undefined_err_to_lv_error_cluster_ptr(LV_ErrorClusterPtr_t, const char*);
        LV_MgErr_t caught_exception_to_lv_err(std::exception_ptr,LV_ErrorClusterPtr_t, const char*);
    }
}

#endif //G_AR_TK__INTEROP_LV_ERROR_HPP_