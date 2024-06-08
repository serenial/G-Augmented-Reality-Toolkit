#ifndef G_AR_TK__CAPTURE_CONTEXT_HPP_
#define G_AR_TK__CAPTURE_CONTEXT_HPP_

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <map>
#include <utility>

#include "g_ar_toolkit/lv_interop/lv_types.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        // forward decleration of Stream
        class Stream;

        struct stream_type_t
        {
            uint32_t width, height, fps_numerator, fps_denominator;
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
            virtual void enumerate_devices(std::vector<device_info_t> &) =0;
            virtual ~Context() = default;
            Stream* open_stream(std::string_view, stream_type_t);

        };

        // context factory
        // this should be provided by the platform specific implementation
        Context* create_platform_context();
    }
}

#endif // G_AR_TK__CAPTURE_CONTEXT_HPP_