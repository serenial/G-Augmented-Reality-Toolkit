#ifndef G_AR_TK__CAPTURE_CONTEXT_HPP_
#define G_AR_TK__CAPTURE_CONTEXT_HPP_

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "g_ar_toolkit/lv-interop/lv-types.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        // forward decleration of Stream
        class Stream;

        struct fractional_fps_t
        {
            uint32_t numerator, denominator;
        };

        enum class stream_pixel_format : uint8_t
        {
            UNKNOWN,
            RGB24,
            RGB32,
            YUY2,
            NV12,
            MJPEG,
            H264
        };

        struct stream_type_t
        {
            stream_pixel_format format;
            uint32_t width, height;
            fractional_fps_t fps;
        };

        struct device_info_t
        {
            std::string device_id;
            std::string device_name;
            std::vector<stream_type_t> supported_formats;
        };

        class Context
        {
        public:
            Context();
            virtual ~Context();
            virtual void enumerate_devices(std::vector<device_info_t> &);
            Stream* open_stream(std::string, stream_type_t);
        };

        // context factory
        Context* create_platform_context();
    }
}

#endif // G_AR_TK__CAPTURE_CONTEXT_HPP_