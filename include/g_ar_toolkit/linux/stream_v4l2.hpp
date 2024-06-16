#ifndef G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_
#define G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_

#ifdef __linux__

#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "../capture/stream.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class StreamV4L2 : public g_ar_toolkit::capture::Stream
        {
        public:
            StreamV4L2(std::string_view, stream_type_t);
            void capture_frame(cv::Mat &, std::chrono::milliseconds);
            void start_stream();
            void stop_stream();
            ~StreamV4L2();

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



            int m_fd; // device handle
            std::mutex m_mtx;
            std::condition_variable m_notifier;
            states m_last_state;
            cv::Mat m_buffer_mat;
            cv::Mat *m_dest_mat_ptr;
            //const std::future<void> m_ftr;
            //const uint32_t m_rows, m_cols;
            std::exception_ptr m_last_exception;
            bool m_streaming;
        };
    }
}

#endif //__linux__
#endif // G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_