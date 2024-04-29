#ifndef G_AR_TK__CAPTURE_STREAM_HPP_
#define G_AR_TK__CAPTURE_STREAM_HPP_

#include <memory>
#include <string>
#include <map>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "./context.hpp"

namespace g_ar_toolkit{
    namespace capture{
        
        class Stream{
            public:
            Stream();
            virtual ~Stream();
            virtual void start_stream();
            virtual void stop_stream();
            virtual void capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout);
        };

        // stream factory
        Stream* create_platform_stream(std::string, stream_type_t stream_type);
    }
}

#endif //G_AR_TK__CAPTURE_STREAM_HPP_