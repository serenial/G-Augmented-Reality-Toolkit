#ifdef __linux__
#ifndef G_AR_TK__INTEROP_LV_CONTEXTV4L2_FORMATS_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTV4L2_FORMATS_HPP_

#include <string>
#include <tuple>

extern "C"
{
#include "linux/videodev2.h"
}

namespace g_ar_toolkit
{
    namespace capture
    {
        static const std::tuple<__u32, std::string, std::string> formats_guid_and_names[] =
            {
                {V4L2_PIX_FMT_YUYV, "YUYV", "yuyv2rgb"},
                {V4L2_PIX_FMT_UYVY, "UYVY", "uyvy2rgb"},
                {V4L2_PIX_FMT_RGB332, "RGB332", "rgb8"},
                {V4L2_PIX_FMT_MJPEG, "MJPEG", "mjpeg2rgb"},
                {V4L2_PIX_FMT_M420, "M420", "m4202rgb"}
            };
    }
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_FORMATS_HPP_
#endif //__linux__
