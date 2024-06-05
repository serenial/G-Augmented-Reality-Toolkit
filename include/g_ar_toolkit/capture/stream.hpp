#ifndef G_AR_TK__CAPTURE_STREAM_HPP_
#define G_AR_TK__CAPTURE_STREAM_HPP_

#include <memory>
#include <string>
#include <map>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "./context.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {

        class Stream
        {
        public:
            virtual void start_stream() = 0;
            virtual void stop_stream() = 0;
            virtual void capture_frame(cv::Mat &, std::chrono::milliseconds) = 0;
            virtual ~Stream() = default;
        };

        // stream factory
        // this should be provided by the platform specific implementations
        Stream *create_platform_stream(const std::string&, stream_type_t, uint32_t);
    }
}

#endif // G_AR_TK__CAPTURE_STREAM_HPP_