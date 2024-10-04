#ifndef G_AR_TK__INTEROP_LV_STREAMWMF_HPP_
#define G_AR_TK__INTEROP_LV_STREAMWMF_HPP_

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
        public:
            StreamWMF(std::string_view, stream_type_t);
            ~StreamWMF();
            void capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout) override;
            void start_stream() override;
            void stop_stream() override;
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

            std::mutex m_mtx;
            std::condition_variable m_notifier;
            states m_last_state;
            cv::Mat m_buffer_mat;
            cv::Mat* m_dest_mat_ptr;
            const std::future<void> m_ftr;
            winrt::event_token m_sample_handler_token;
            const uint32_t m_rows,m_cols;
            std::exception_ptr m_last_exception;
            bool m_streaming;

            void on_sample(IMFSample *);
        };

    }
}

#endif //_WIN32
#endif //G_AR_TK__INTEROP_LV_STREAMWMF_HPP_