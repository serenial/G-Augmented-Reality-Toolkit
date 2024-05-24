#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-str.hpp"
#include "g_ar_toolkit/lv-interop/lv-array.hpp"
#include "g_ar_toolkit/lv-interop/lv-edvr-managed-object.hpp"
#include "g_ar_toolkit_export.h"

#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace lv_interop;

#include "g_ar_toolkit/lv-interop/set-packing.hpp"

using LV_StreamFormatParameters_t =struct
{
    uint32_t width, height, fps_numerator, fps_denominator, pixel_format;
};

// array allocation for this struct should use PtrSized type!
using LV_DeviceInfo_t = struct
{
    LV_StringHandle_t id;
    LV_StringHandle_t name;
    LV_Handle_t<LV_Array_t<1, LV_StreamFormatParameters_t>> formats;
};

using LV_DeviceInfoHandlePtr_t = LV_HandlePtr_t<LV_Array_t<1, LV_DeviceInfo_t>>;

using LV_FormatListItem_t = struct {
    uint32_t index;
    LV_StringHandle_t name;
};

using LV_FormatListHandlePtr_t = LV_HandlePtr_t<LV_Array_t<1, LV_FormatListItem_t>>;

#include "g_ar_toolkit/lv-interop/reset-packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_context_create(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            // create context
            EDVRManagedObject<Context>(edvr_ref_ptr, create_platform_context());
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_context_enumerate(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_DeviceInfoHandlePtr_t device_info_handle_ptr)
    {
        try
        {
            EDVRManagedObject<Context> context(edvr_ref_ptr);

            std::vector<device_info_t> devices;

            context.get_object()->enumerate_devices(devices);

            copy_with_allocation_to_1d_lv_array_handle_ptr<std::vector<device_info_t>, LV_DeviceInfo_t>(
                devices,
                device_info_handle_ptr,
                [](LV_DeviceInfo_t *out_ptr, bool newly_allocated, device_info_t in)
                {
                    if (newly_allocated)
                    {
                        out_ptr->id = 0;
                        out_ptr->name = 0;
                        out_ptr->formats = 0;
                    }
                    copy_std_string_to_lv_string_handle_ptr(in.device_id, &(out_ptr->id));
                    copy_std_string_to_lv_string_handle_ptr(in.device_name, &(out_ptr->name));
                    copy_with_allocation_to_1d_lv_array_handle_ptr<std::vector<stream_type_with_format_t>, LV_StreamFormatParameters_t>(
                        in.supported_formats, 
                        &(out_ptr->formats), 
                        [](LV_StreamFormatParameters_t *out_ptr, bool newly_allocated, stream_type_with_format_t in){
                            out_ptr->fps_numerator = in.stream_type.fps_numerator;
                            out_ptr->fps_denominator = in.stream_type.fps_denominator;
                            out_ptr->height = in.stream_type.height;
                            out_ptr->width = in.stream_type.width;
                            out_ptr->pixel_format = in.pixel_format; } //no-deallcoator required for these types
                        ); 
                },
                [](LV_DeviceInfo_t to_deallocate)
                {
                    // dispose of unused string and array handles
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(to_deallocate.id));
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(to_deallocate.name));
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(to_deallocate.formats));
                });
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_context_list_formats(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_FormatListHandlePtr_t format_list_handle_ptr)
    {
        try
        {
            EDVRManagedObject<Context> context(edvr_ref_ptr);

            std::vector<format_item_t> list;

            context.get_object()->list_of_formats(list);

            copy_with_allocation_to_1d_lv_array_handle_ptr<std::vector<format_item_t>, LV_FormatListItem_t>(
                list,
                format_list_handle_ptr,
                [](LV_FormatListItem_t *out_ptr, bool newly_allocated, format_item_t in)
                {
                    if (newly_allocated)
                    {
                        out_ptr->name = 0;
                    }
                    out_ptr->index = in.index;
                    copy_std_string_to_lv_string_handle_ptr(in.name, &(out_ptr->name));
                },
                [](LV_FormatListItem_t to_deallocate)
                {
                    // dispose of unused string handles
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(to_deallocate.name));
                });
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}

Stream* Context::open_stream(std::string device_id, stream_type_t stream_format, uint32_t options)
{
    return create_platform_stream(device_id, stream_format, options);
}
Context::Context() {}
Context::~Context() {}

void Context::enumerate_devices(std::vector<device_info_t> &devices) {}
void Context::list_of_formats(std::vector<format_item_t> &list){}