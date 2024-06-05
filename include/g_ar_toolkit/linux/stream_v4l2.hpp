#ifndef G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_
#define G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_

#ifdef __linux__

#include <memory>
#include "usb_cam/usb_cam.hpp"
#include "usb_cam/utils.hpp"

#include "../capture/stream.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        struct options_bitfield_t
        {
            uint32_t pixel_format : 8;
            uint32_t io_method : 2;
            uint32_t packing : 22;
        };
        union options_t
        {
            uint32_t as_value;
            options_bitfield_t as_bits;
        };
        class StreamV4L2 : public g_ar_toolkit::capture::Stream
        {
        public:
            StreamV4L2(const std::string&, stream_type_t, uint32_t);
            void capture_frame(cv::Mat &, std::chrono::milliseconds);
            void start_stream();
            void stop_stream();
            ~StreamV4L2();

        private:
            const std::unique_ptr<usb_cam::UsbCam> usb_cam_ptr;
            cv::Mat rgb_buffer;
        };
    }
}

#endif //__linux__
#endif // G_AR_TK__INTEROP_LV_STREAMV4L2_HPP_