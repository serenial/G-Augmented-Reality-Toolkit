#pragma once

#ifdef __linux__

#include "../capture/stream.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class StreamV4L2 : public g_ar_toolkit::capture::Stream
        {
        public:
            StreamV4L2(std::string, stream_type_t);
            ~StreamV4L2();
            void capture_frame(cv::Mat&, std::chrono::milliseconds);
            void start_stream();
            void stop_stream();
        };
    }
}

#endif