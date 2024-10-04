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

            void enqueue_buffer(int);
            int dequeue_buffer();

            const stream_type_t m_stream_type;
            const std::string_view m_device_id;
            scoped_file_descriptor m_scoped_fd; // device handle
            const __u32 m_pixel_format;
            const std::unique_ptr<decoder> m_decoder;
            const std::vector<scoped_mmap_buffer> m_buffer_list;

            // std::mutex m_mtx;
            // std::condition_variable m_notifier;
            bool m_is_streaming;
        };
    }
}

#endif //__linux__
#endif // G_AR_TK__CAPTURE_LV_STREAMV4L2_HPP_