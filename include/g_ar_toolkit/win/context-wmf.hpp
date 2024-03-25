#pragma once

#include <stdexcept>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <future>

#include <windows.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <Rpc.h>
#include <mfreadwrite.h>

#include "../capture/context.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class ContextWMF : public g_ar_toolkit::capture::Context
        {
            enum class states
            {
                STARTING,
                WAITING_ON_ACTION,
                LISTING_DEVICES,
                STOPPING,
                STOPPED
            };
            enum class errors
            {
                NO_ERR,
                COM_INIT_ERR,
                MF_STARTUP_BAD_VER,
                MF_STARTUP_OTHER_ERR,
                DEVICE_ENUM_ERROR
            };
            states last_state;
            errors last_error;
            std::mutex mtx;
            const std::future<HRESULT> ftr;
            std::condition_variable notifier;
            std::vector<device_info_t> last_enumeration;
            void update_last_device_enumeration();

        public:
            ContextWMF();
            ~ContextWMF();
            void enumerate_devices(std::vector<device_info_t> &devices);
        };

        struct GUIDHash
        {
            std::size_t operator()(const GUID &g) const
            {

                RPC_STATUS status;
                UUID uuid = g;
                auto hash = UuidHash(&uuid, &status);
                if (status != RPC_S_OK)
                {
                    throw(std::out_of_range("Unable to generate hash of GUID"));
                }
                return hash;
            }
        };

        static const std::unordered_map<GUID, capture::stream_pixel_format, GUIDHash>
            wmf_pixel_format_to_context_pixel_format_map =
                {
                    {MFVideoFormat_NV12, capture::stream_pixel_format::NV12},
                    {MFVideoFormat_MJPG, capture::stream_pixel_format::MJPEG},
                    {MFVideoFormat_RGB24, capture::stream_pixel_format::RGB24},
                    {MFVideoFormat_RGB32, capture::stream_pixel_format::RGB32},
                    {MFVideoFormat_YUY2, capture::stream_pixel_format::YUY2},
                    {MFVideoFormat_H264, capture::stream_pixel_format::H264}};

        static const std::unordered_map<stream_pixel_format, GUID>
            context_pixel_format_to_wmf_pixel_format_map =
                {
                    {capture::stream_pixel_format::NV12, MFVideoFormat_NV12},
                    {capture::stream_pixel_format::MJPEG, MFVideoFormat_MJPG},
                    {capture::stream_pixel_format::RGB24, MFVideoFormat_RGB24},
                    {capture::stream_pixel_format::RGB32, MFVideoFormat_RGB32},
                    {capture::stream_pixel_format::YUY2, MFVideoFormat_YUY2},
                    {capture::stream_pixel_format::H264, MFVideoFormat_H264}};
    }

    template<typename T>
    struct CoTaskMemFreeDeleter {
    void operator()(T* p) const
    {
        CoTaskMemFree(*p);
    }
};
}