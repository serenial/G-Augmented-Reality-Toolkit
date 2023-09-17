#pragma once

#ifdef _WIN32
// include windows.h
#include <windows.h>
// include windows media foundation headers whilst we are here
#include <mfapi.h>
#else
#include <dlfcn.h>
#endif

#include "./lv-types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        LV_MgErr_t EDVR_GetCurrentContext(LV_Ptr_t<LV_EDVRContext_t>);
        LV_MgErr_t EDVR_CreateReference(LV_EDVRReferencePtr_t, LV_EDVRDataHandle_t);
        LV_MgErr_t EDVR_AddRefWithContext(LV_EDVRReference_t, LV_EDVRContext_t, LV_EDVRDataHandle_t);
        LV_MgErr_t EDVR_ReleaseRefWithContext(LV_EDVRReference_t, LV_EDVRContext_t);
        LV_MgErr_t PostLVUserEvent(LV_UserEventRef_t, void *);
        LV_MgErr_t NumericArrayResize(int32_t type, int32_t n_dims, LV_UHandlePtr_t, size_t size);
        LV_MgErr_t DSDisposeHandle(LV_UHandle_t);

        using LV_NumericType_t = int32_t; 
        const LV_NumericType_t LV_NumericType_I8 = 0x01;
        const LV_NumericType_t LV_NumericType_I16 = 0x02;
        const LV_NumericType_t LV_NumericType_I32 = 0x03;
        const LV_NumericType_t LV_NumericType_I64 = 0x04;
        const LV_NumericType_t LV_NumericType_U8 = 0x05;
        const LV_NumericType_t LV_NumericType_U16 = 0x06;
        const LV_NumericType_t LV_NumericType_U32 = 0x07;
        const LV_NumericType_t LV_NumericType_U64 = 0x08;
        const LV_NumericType_t LV_NumericType_SGL = 0x09;
        const LV_NumericType_t LV_NumericType_DBL = 0x0A;
        const LV_NumericType_t LV_NumericType_EXT = 0x0B;
        const LV_NumericType_t LV_NumericType_CSG = 0x0C;
        const LV_NumericType_t LV_NumericType_CDB = 0x0D;
        const LV_NumericType_t LV_NumericType_CXT = 0x0E;

        LV_MgErr_t get_edvr_data_handle_with_context(LV_EDVRReference_t, LV_EDVRContext_t *, LV_EDVRDataHandle_t);
    }
}