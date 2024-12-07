#include "g_ar_toolkit/lv_interop/lv_error.hpp"

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

class LV_StreamFormatParameters_t
{
    uint32_t width, height, fps_numerator, fps_denominator;

public:
    LV_StreamFormatParameters_t &operator=(const Stream::stream_type_t stream_type)
    {
        width = stream_type.width;
        height = stream_type.height;
        fps_numerator = stream_type.fps_numerator;
        fps_denominator = stream_type.fps_denominator;
        return *this;
    }
};

// array allocation for this struct should use PtrSized type!
using LV_DeviceInfo_t = struct
{
    LV_StringHandle_t id;
    LV_StringHandle_t name;
    LV_1DArrayHandle_t<LV_StreamFormatParameters_t> formats;
};

using LV_DeviceInfoHandlePtr_t = LV_HandlePtr_t<LV_Array_t<1, LV_DeviceInfo_t>>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_context_create(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            // create context
            EDVRManagedObject<Context>(edvr_ref_ptr, new Context());
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_context_enumerate(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_DeviceInfo_t> device_info_handle)
    {
        try
        {
            EDVRManagedObject<Context> context(edvr_ref_ptr);

            std::vector<Context::device_info_t> devices;

            context.get_object()->enumerate_devices(devices);

            device_info_handle.copy_element_by_element_from(devices, [](auto device, auto dest)
                                                            {
                                            dest->id.copy_from(device.device_id);
                                            dest->name.copy_from(device.device_name);
                                            dest->formats.copy_element_by_element_from(device.supported_formats, 
                                                [](auto const &f, auto dest_f){*dest_f=f;})
                    ; }, [](LV_DeviceInfo_t to_deallocate)
                                                            {
                    // dispose of unused string and array handles
                    to_deallocate.id.dispose();
                    to_deallocate.name.dispose();
                    to_deallocate.formats.dispose(); });
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}