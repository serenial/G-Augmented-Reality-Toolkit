#pragma once

#ifdef _WIN32

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <future>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

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
                WAITING_ON_STREAM_START,
                WAITING_ON_STREAM_START_ACK,
                WAITING_ON_CAPTURE,
                WAITING_ON_CAPTURE_ACK,
                WAITING_ON_STREAM_STOP,
                WAITING_ON_STREAM_STOP_ACK,
                STOPPING,
                STOPPED
            };

            std::mutex mtx;
            std::condition_variable notifier;
            states last_state;
            cv::Mat buffer_mat;
            cv::Mat* dest_mat_ptr;
            const std::future<void> ftr;
            void on_sample(IMFSample *pSample);
            winrt::event_token sample_handler_token;
            const uint32_t rows,cols;
            std::exception_ptr last_exception;
            bool streaming;
        };

    }
}

#endif