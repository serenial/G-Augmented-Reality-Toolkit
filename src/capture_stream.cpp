#include <type_traits>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

using LV_StreamSpec_t = struct
{
    uint32_t width, height, fps;
};

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_stream_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_capture_ctx_ref_ptr,
        LV_EDVRReferencePtr_t edvr_strm_ref_ptr,
        LV_StringHandle_t device_id_str_handle,
        LV_Ptr_t<LV_StreamSpec_t> stream_spec_ptr,
        uint32_t options)
    {
        try
        {
            EDVRManagedObject<Context> context(edvr_capture_ctx_ref_ptr);

            // convert input to c++ types
            std::string_view device_id = lv_string_handle_to_std_string_view(device_id_str_handle);
            stream_type_t stream_type;
            stream_type.fps_numerator = stream_spec_ptr->fps;
            stream_type.height = stream_spec_ptr->height;
            stream_type.width = stream_spec_ptr->width;
            stream_type.fps_denominator = 1;

            // create stream using context
            auto stream_object = context.get_object()->open_stream(device_id, stream_type, options);
            // wrap stream object into EDVRManaged Object - Use volatile to avoid compiler optimization
            volatile EDVRManagedObject<Stream> stream(edvr_strm_ref_ptr, std::move(stream_object));
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_start(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr)
    {
        try
        {
            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            stream.get_object()->start_stream();
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_stop(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            stream.get_object()->stop_stream();
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_capture_next(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_EDVRReferencePtr_t edvr_image_ref_ptr,
        uint16_t timeout_ms)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            lv_image dst(edvr_image_ref_ptr);

            stream.get_object()->capture_frame(*dst, std::chrono::milliseconds{timeout_ms});
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}