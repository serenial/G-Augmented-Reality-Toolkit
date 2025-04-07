#include <type_traits>
#include <algorithm>
#include <cmath>
#include <limits>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/enumerator.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_StreamFormatParameters_t
    {
        uint16_t width, height;
        uint32_t fps_numerator, fps_denominator;

    public:
        LV_StreamFormatParameters_t &operator=(const Stream::stream_type_t stream_type)
        {
            width = stream_type.width;
            height = stream_type.height;
            fps_numerator = stream_type.fps_numerator;
            fps_denominator = stream_type.fps_denominator;
            return *this;
        }
        operator Stream::stream_type_t() const
        {
            return Stream::stream_type_t{width, height, fps_numerator, fps_denominator};
        }
    };

    // array allocation for this struct should use PtrSized type!
    struct LV_DeviceInfo_t
    {
        LV_StringHandle_t id;
        LV_StringHandle_t name;
        LV_1DArrayHandle_t<LV_StreamFormatParameters_t> formats;
        LV_DeviceInfo_t() = delete;
    };

    using LV_DeviceInfoHandlePtr_t = LV_HandlePtr_t<LV_Array_t<1, LV_DeviceInfo_t>>;

    struct LV_CameraParamInfo_t
    {
        bool supported;
        int32_t max, min, step, default_value;
        LV_CameraParamInfo_t &operator=(const Stream::param_info_t &p)
        {
            default_value = p.default_value;
            max = p.max;
            min = p.min;
            step = p.step;
            supported = p.is_supported;
            return *this;
        }
        LV_CameraParamInfo_t() : supported(false) {};
    };

    using LV_CameraParamInfoPtr_t = LV_Ptr_t<LV_CameraParamInfo_t>;

    class LV_CameraParam_t
    {
    private:
        uint8_t m_value;

    public:
        operator Stream::camera_parameters const()
        {
            const Stream::camera_parameters parameters[] =
                {
                    Stream::camera_parameters::EXPOSURE,
                    Stream::camera_parameters::FOCUS,
                    Stream::camera_parameters::ZOOM,
                    Stream::camera_parameters::GAIN,
                    Stream::camera_parameters::WHITE_BALANCE_TEMPERATURE,
                    Stream::camera_parameters::BRIGHTNESS,
                    Stream::camera_parameters::CONTRAST,
                    Stream::camera_parameters::SATURATION,
                    Stream::camera_parameters::GAMMA,
                    Stream::camera_parameters::HUE,
                    Stream::camera_parameters::SHARPNESS,
                    Stream::camera_parameters::BACKLIGHT_COMPENSATION,
                    Stream::camera_parameters::POWER_LINE_FREQUENCY};

            if (m_value < std::size(parameters))
            {
                return parameters[m_value];
            }

            throw std::out_of_range("The supplied value for the camera parameter does not map to a camera-parameter value.");
        }
    };

    class LV_CameraAutoMode_t
    {
    private:
        uint8_t m_value;

    public:
        operator Stream::camera_auto_parameters const()
        {
            const Stream::camera_auto_parameters modes[] =
                {
                    Stream::camera_auto_parameters::AUTO_EXPOSURE,
                    Stream::camera_auto_parameters::AUTO_FOCUS,
                    Stream::camera_auto_parameters::AUTO_WHITE_BALANCE,
                    Stream::camera_auto_parameters::AUTO_GAIN};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the camera auto mode does not map to a camera-auto-mode value.");
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

}

extern "C"
{

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_enumerate_devices(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<LV_DeviceInfo_t> device_info_handle)
    {
        try
        {
            Enumerator enumerator{};

            std::vector<Enumerator::device_info_t> devices;

            enumerator.enumerate_devices(devices);

            device_info_handle.copy_element_by_element_from(devices, [](auto device, auto dest)
                                                            {
                                            dest->id.copy_from(device.device_id);
                                            dest->name.copy_from(device.device_name);
                                            dest->formats.copy_element_by_element_from(device.supported_formats, 
                                                [](auto const &f, auto dest_f){*dest_f=f;})
                    ; });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_strm_ref_ptr,
        LV_StringHandle_t device_id_str_handle,
        LV_Ptr_t<LV_StreamFormatParameters_t> stream_spec_ptr)
    {
        try
        {
            // convert input to c++ types
            std::string_view device_id = device_id_str_handle;

            EDVRManagedObject<Stream> stream(edvr_strm_ref_ptr, new Stream(device_id, *stream_spec_ptr));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
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

            stream->start_stream();
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
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

            stream->stop_stream();
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_capture_next(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_EDVRReferencePtr_t edvr_image_ref_ptr,
        LV_BooleanPtr_t timeout,
        uint16_t timeout_ms)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            lv_image dst(edvr_image_ref_ptr);

            *timeout = stream->capture_frame(*dst, std::chrono::milliseconds{timeout_ms});
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_get_param_info(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_CameraParam_t cam_param,
        LV_CameraParamInfoPtr_t cam_param_info_ptr)
    {
        try
        {
            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            Stream::param_info_t info;

            stream->get_camera_parameter_info(cam_param, &info);

            *cam_param_info_ptr = info;
        }
        catch (Stream::param_error &e)
        {
            *cam_param_info_ptr = LV_CameraParamInfo_t{};
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_get_param_value(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_CameraParam_t cam_param,
        int32_t *value)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            *value = stream->get_camera_parameter(cam_param);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_set_param_value(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_CameraParam_t cam_param,
        int32_t *value)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);

            Stream::param_info_t info;
            stream->get_camera_parameter_info(cam_param, &info);

            int32_t quantized = std::round((*value - info.min) / info.step) * info.step + info.min;
            int32_t clamped = std::clamp(quantized, info.min, info.max);

            *value = clamped;

            stream->set_camera_parameter(cam_param, clamped);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_get_auto_param_value(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_CameraAutoMode_t cam_auto_param,
        LV_BooleanPtr_t is_automatic,
        LV_BooleanPtr_t is_supported)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);
            *is_automatic = stream->get_camera_auto_mode(cam_auto_param);
            *is_supported = true;
        }
        catch (Stream::auto_param_error &e)
        {
            *is_supported = false;
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_stream_set_auto_param_value(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_stream_ref_ptr,
        LV_CameraAutoMode_t cam_auto_param,
        LV_BooleanPtr_t use_automatic)
    {
        try
        {

            EDVRManagedObject<Stream> stream(edvr_stream_ref_ptr);
            stream->set_camera_auto_mode(cam_auto_param, *use_automatic);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}