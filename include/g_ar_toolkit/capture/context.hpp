#ifndef G_AR_TK__CAPTURE_CONTEXT_HPP_
#define G_AR_TK__CAPTURE_CONTEXT_HPP_

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <utility>

#include "g_ar_toolkit/lv-interop/lv-types.hpp"

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

        struct stream_type_with_format_t
        {
            stream_type_t stream_type;
            uint32_t pixel_format;
        };

        struct device_info_t
        {
            std::string device_id;
            std::string device_name;
            std::vector<stream_type_with_format_t> supported_formats;
        };

        struct format_item_t {
            uint32_t index;
            std::string name;
        };

        class Context
        {
        public:
            virtual void enumerate_devices(std::vector<device_info_t> &) =0;
            virtual void list_of_formats(std::vector<format_item_t>&) = 0;
            Stream* open_stream(std::string, stream_type_t, uint32_t);
        };

        // context factory
        // this should be provided by the platform specific implementation
        Context* create_platform_context();
        
        // utility functions
        uint32_t get_pixel_format_from_options(uint32_t);
    }
}

#endif // G_AR_TK__CAPTURE_CONTEXT_HPP_