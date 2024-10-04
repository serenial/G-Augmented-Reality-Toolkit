#ifndef G_AR_TK__CAPTURE_LV_STREAMV4L2_HPP_
#define G_AR_TK__CAPTURE_LV_STREAMV4L2_HPP_

#ifdef __linux__

#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "../capture/stream.hpp"
#include "./decoders_v4l2.hpp"
#include "./utils_v4l2.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class StreamV4L2 : public g_ar_toolkit::capture::Stream
        {
        public:
            StreamV4L2(std::string_view, stream_type_t);
            void capture_frame(cv::Mat &, std::chrono::milliseconds) override;
            void start_stream() override;
            void stop_stream() override;
            ~StreamV4L2();

        private:
            StreamV4L2(std::string_view, stream_type_t, std::pair<scoped_file_descriptor, __u32>);
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


            const stream_type_t m_stream_type;
            scoped_file_descriptor m_scoped_fd; // device handle
            const __u32 m_pixel_format;
            std::mutex m_mtx;
            std::condition_variable m_notifier;

            states m_last_state;
            cv::Mat m_buffer_mat;
            //const std::future<void> m_ftr;
            //const uint32_t m_rows, m_cols;
            std::exception_ptr m_last_exception;
            bool m_streaming;
        };
    }
}

#endif //__linux__
#endif // G_AR_TK__CAPTURE_LV_STREAMV4L2_HPP_