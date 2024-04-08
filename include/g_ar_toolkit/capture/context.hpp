#pragma once

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

        using fractional_fps_t = struct
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

        using stream_type_t = struct
        {
            stream_pixel_format format;
            uint32_t width, height;
            fractional_fps_t fps;
        };

        using device_info_t = struct
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
