#ifndef G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_

#ifdef __linux__

#include <chrono>

#include "../capture/context.hpp"

extern "C"
{
#include <linux/videodev2.h>
}

namespace g_ar_toolkit
{
    namespace capture
    {
        bool fourcc_is_a_stream_pixel_format_match(stream_pixel_format, __u32);
        void lookup_support_formats_by_device_path(std::string, std::vector<v4l2_frmivalenum> &);
        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {
        public:
            ContextV4L2();
            ~ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices);
        };
    };
}

#endif // __linux__

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_