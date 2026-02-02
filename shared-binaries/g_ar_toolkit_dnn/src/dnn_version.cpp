#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit_dnn_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_DNN_EXPORT LV_MgErr_t g_ar_tk_dnn_verson(LV_StringHandle_t ver_handle)
    {
        try
        {
            ver_handle.copy_from(G_AR_TOOLKIT_VERSION);
        }
        catch (...)
        {
            return LV_ERR_bogusError;
        }
        return LV_ERR_noError;
    }
}