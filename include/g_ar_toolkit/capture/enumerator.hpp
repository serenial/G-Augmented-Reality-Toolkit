#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <map>
#include <utility>

#include "g_ar_toolkit/lv_interop/lv_types.hpp"

#include "./stream.hpp"

#ifdef _WIN32
#include <mutex>
#include <condition_variable>
#include <future>

#include <windows.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <Rpc.h>
#include <mfreadwrite.h>
#endif

#ifdef __linux__
#include <linux/videodev2.h>
#endif

namespace g_ar_toolkit
{
    namespace capture
    {
        class Enumerator
        {
        public:
            struct device_info_t
            {
                std::string device_id;
                std::string device_name;
                std::vector<Stream::stream_type_t> supported_formats;
            };
            void enumerate_devices(std::vector<device_info_t> &);
#ifdef _WIN32
            public:
            Enumerator();
            ~Enumerator();
            private:
            // windows specific members and methods
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
            states m_last_state;
            errors m_last_error;
            std::mutex m_mtx;
            const std::future<HRESULT> m_ftr;
            std::condition_variable m_notifier;
            std::vector<device_info_t> m_last_enumeration;
            void update_last_device_enumeration();
#endif
#ifdef __linux__
            // Linux Specific Members and Methods
#endif
        };
    }
}