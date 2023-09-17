#pragma once

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <future>

#include <windows.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <mfreadwrite.h>
#include <winrt/base.h>

#include "../capture/stream.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class StreamWMF : public g_ar_toolkit::capture::Stream
        {
        private:
            enum class states
            {
                STARTING,
                WAITING_ON_ACTION,
                STOPPING,
                STOPPED
            };
            enum class errors
            {
                NO_ERR,
                COM_INIT_ERR,
                MF_STARTUP_BAD_VER,
                MF_STARTUP_OTHER_ERR,
                CAPTURE_DEVICE_ID_NOT_FOUND,
                CAPTURE_DEVICE_FORMAT_INVALID,
                CAPTURE_DEVICE_FPS_INVALID,
                CAPTURE_DEVICE_DIMS_INVALID,
                CAPTURE_STREAM_APPLYING_CONFIG
            };

            std::mutex mtx;
            std::condition_variable cv;
            states last_state;
            errors last_error;
            const std::future<void> ftr;

        public:
            StreamWMF(std::string device_id, stream_type_t stream_format);
            ~StreamWMF();
            bool grab_frame(cv::Mat &destination, std::chrono::milliseconds timeout);
            bool start();
            bool stop();
        };

    }
}