#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

// static function pointers
static LV_EDVRGetCurrentContextFnPtr_t EDVR_GetCurrentContextImp = nullptr;
static LV_EDVRCreateReferenceFnPtr_t EDVR_CreateReferenceImp = nullptr;
static LV_EDVRAddRefWithContextFnPtr_t EDVR_AddRefWithContextImp = nullptr;
static LV_EDVRReleaseRefWithContextFnPtr_t EDVR_ReleaseRefWithContextImp = nullptr;
static LV_PostLVUserEventFnPtr_t PostLVUserEventImp = nullptr;
static LV_NumericArrayResizeFnPtr_t NumericArrayResizeImp = nullptr;
static LV_DSDisposeHandleFnPtr_t DSDisposeHandleImp = nullptr;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_intialize_functions(LV_InstanceDataHandle_t instance_data_handle)
    {
        for (int i = 0; i < 2; i++)
        {
            // check if the function pointers already populated
            if (EDVR_GetCurrentContextImp && EDVR_CreateReferenceImp && EDVR_AddRefWithContextImp && EDVR_ReleaseRefWithContextImp && PostLVUserEventImp && NumericArrayResizeImp && DSDisposeHandleImp)
            {
                return LV_ERR_noError;
            }

            if (i == 0)
            {
#ifdef _WIN32
                auto module = GetModuleHandle("LabVIEW.exe");
                if (!module)
                {
                    module = GetModuleHandle("lvffrt.dll");
                }
                if (!module)
                {
                    module = GetModuleHandle("lvrt.dll");
                }
                if (!module)
                {
                    return LV_ERR_rfNotFound;
                }

                EDVR_GetCurrentContextImp = reinterpret_cast<LV_EDVRGetCurrentContextFnPtr_t>(GetProcAddress(module, "EDVR_GetCurrentContext"));
                EDVR_CreateReferenceImp = reinterpret_cast<LV_EDVRCreateReferenceFnPtr_t>(GetProcAddress(module, "EDVR_CreateReference"));
                EDVR_AddRefWithContextImp = reinterpret_cast<LV_EDVRAddRefWithContextFnPtr_t>(GetProcAddress(module, "EDVR_AddRefWithContext"));
                EDVR_ReleaseRefWithContextImp = reinterpret_cast<LV_EDVRReleaseRefWithContextFnPtr_t>(GetProcAddress(module, "EDVR_ReleaseRefWithContext"));
                PostLVUserEventImp = reinterpret_cast<LV_PostLVUserEventFnPtr_t>(GetProcAddress(module, "PostLVUserEvent"));
                NumericArrayResizeImp = reinterpret_cast<LV_NumericArrayResizeFnPtr_t>(GetProcAddress(module, "NumericArrayResize"));
                DSDisposeHandleImp = reinterpret_cast<LV_DSDisposeHandleFnPtr_t>(GetProcAddress(module, "DSDisposeHandle"));
            }
            else
            {
                return LV_ERR_rfNotFound;
            }
#else

                auto module = dlopen(nullptr, RTLD_LAZY);

                if (!module)
                {
                    return LV_ERR_rfNotFound;
                }

                EDVR_GetCurrentContextImp = reinterpret_cast<LV_EDVRGetCurrentContextFnPtr_t>(dlsym(module, "EDVR_GetCurrentContext"));
                EDVR_CreateReferenceImp = reinterpret_cast<LV_EDVRCreateReferenceFnPtr_t>(dlsym(module, "EDVR_CreateReference"));
                EDVR_AddRefWithContextImp = reinterpret_cast<LV_EDVRAddRefWithContextFnPtr_t>(dlsym(module, "EDVR_AddRefWithContext"));
                EDVR_ReleaseRefWithContextImp = reinterpret_cast<LV_EDVRReleaseRefWithContextFnPtr_t>(dlsym(module, "EDVR_ReleaseRefWithContext"));
                PostLVUserEventImp = reinterpret_cast<LV_PostLVUserEventFnPtr_t>(dlsym(module, "PostLVUserEvent"));
                NumericArrayResizeImp = reinterpret_cast<LV_NumericArrayResizeFnPtr_t>(dlsym(module, "NumericArrayResize"));
                DSDisposeHandleImp = reinterpret_cast<LV_DSDisposeHandleFnPtr_t>(dlsym(module, "DSDisposeHandle"));
#endif
        }

        return LV_ERR_bogusError;
    }
}

LV_MgErr_t g_ar_toolkit::lv_interop::EDVR_GetCurrentContext(LV_Ptr_t<LV_EDVRContext_t> ctx_ptr)
{
    try
    {
        return EDVR_GetCurrentContextImp(ctx_ptr);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::EDVR_CreateReference(LV_EDVRReferencePtr_t edvr_ref_ptr, LV_EDVRDataHandle_t hndl)
{
    try
    {
        return EDVR_CreateReferenceImp(edvr_ref_ptr, hndl);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::EDVR_AddRefWithContext(LV_EDVRReference_t edvr_ref_ptr, LV_EDVRContext_t ctx, LV_EDVRDataHandle_t hndl)
{
    try
    {
        return EDVR_AddRefWithContextImp(edvr_ref_ptr, ctx, hndl);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::EDVR_ReleaseRefWithContext(LV_EDVRReference_t edvr_ref_ptr, LV_EDVRContext_t ctx)
{
    try
    {
        return EDVR_ReleaseRefWithContextImp(edvr_ref_ptr, ctx);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::PostLVUserEvent(LV_UserEventRef_t ue_ref, void *data_ptr)
{
    try
    {
        return PostLVUserEventImp(ue_ref, data_ptr);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::NumericArrayResize(int32_t type, int32_t n_dims, LV_UHandlePtr_t hndl, size_t size)
{
    try
    {
        return NumericArrayResizeImp(type, n_dims, hndl, size);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}
LV_MgErr_t g_ar_toolkit::lv_interop::DSDisposeHandle(LV_UHandle_t hndl)
{
    try
    {
        return DSDisposeHandleImp(hndl);
    }
    catch (...)
    {
        return LV_ERR_bogusError;
    }
}

LV_MgErr_t g_ar_toolkit::lv_interop::get_edvr_data_handle_with_context(LV_EDVRReference_t edvr_ref, LV_EDVRContext_t *cntx_ptr, LV_EDVRDataHandle_t data_handle)
{

    if (!edvr_ref || !cntx_ptr || !data_handle)
    {
        return LV_ERR_mgArgErr;
    }

    auto err = EDVR_GetCurrentContext(cntx_ptr);

    if (err)
    {
        return err;
    }

    if (cntx_ptr == 0)
    {
        return LV_ERR_bogusError;
    }

    err = EDVR_AddRefWithContext(edvr_ref, *cntx_ptr, data_handle);

    return err;
}