#ifndef G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_
#define G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_

#ifdef __linux__

#include <memory>

#include "../capture/stream.hpp"
#include "usb_cam/usb_cam.hpp"

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
        private:
            const std::unique_ptr<usb_cam::UsbCam> usb_cam_ptr;
            usb_cam::parameters_t usb_cam_parameters;
        };
    }
}

#endif //__linux__
#endif //G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_