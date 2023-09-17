#pragma once

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
            virtual bool start();
            virtual bool stop();
            virtual bool grab_frame(cv::Mat &destination, std::chrono::milliseconds timeout);
        };

        Stream* create_platform_stream(std::string, stream_type_t stream_type);
    }
}