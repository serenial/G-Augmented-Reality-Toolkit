#ifndef G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_
#define G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_

#ifdef __linux__

#include <memory>

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
        };
    }
}

#endif //__linux__
#endif // G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_