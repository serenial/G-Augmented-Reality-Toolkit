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
#include "g_ar_toolkit/capture/context.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

struct LV_StreamSpec_t
{
    uint32_t width, height, fps;
};

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
    LV_CameraParamInfo_t(): supported(false){};
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

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_capture_stream_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_capture_ctx_ref_ptr,
        LV_EDVRReferencePtr_t edvr_strm_ref_ptr,
        LV_StringHandle_t device_id_str_handle,
        LV_Ptr_t<LV_StreamSpec_t> stream_spec_ptr)
    {
        try
        {
            EDVRManagedObject<Context> context(edvr_capture_ctx_ref_ptr);

            // convert input to c++ types
            std::string_view device_id = device_id_str_handle;
            Stream::stream_type_t stream_type;
            stream_type.fps_numerator = stream_spec_ptr->fps;
            stream_type.height = stream_spec_ptr->height;
            stream_type.width = stream_spec_ptr->width;
            stream_type.fps_denominator = 1;

            // create stream using context
            auto stream_object = context.get_object()->open_stream(device_id, stream_type);
            // wrap stream object into EDVRManaged Object - Use volatile to avoid compiler optimization
            volatile EDVRManagedObject<Stream> stream(edvr_strm_ref_ptr, std::move(stream_object));
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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

            stream.get_object()->get_camera_parameter_info(cam_param, &info);

            *cam_param_info_ptr = info;
        }
        catch (Stream::param_error &e)
        {
            *cam_param_info_ptr = LV_CameraParamInfo_t{};
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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

            *value = stream.get_object()->get_camera_parameter(cam_param);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            stream.get_object()->get_camera_parameter_info(cam_param, &info);

            int32_t quantized = std::round((*value - info.min) / info.step) * info.step + info.min;
            int32_t clamped = std::clamp(quantized, info.min, info.max);

            *value = clamped;

            stream.get_object()->set_camera_parameter(cam_param, clamped);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            *is_automatic = stream.get_object()->get_camera_auto_mode(cam_auto_param);
            *is_supported = true;
        }
        catch (Stream::auto_param_error &e)
        {
            *is_supported = false;
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
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
            stream.get_object()->set_camera_auto_mode(cam_auto_param, *use_automatic);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}