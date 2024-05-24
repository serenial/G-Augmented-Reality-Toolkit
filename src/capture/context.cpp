#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;

Stream *Context::open_stream(std::string device_id, stream_type_t stream_format, uint32_t options)
{
    return create_platform_stream(device_id, stream_format, options);
}

uint32_t capture::get_pixel_format_from_options(uint32_t options)
{
    // pixel_format is bits 7 to 0
    return options & 0x000000FF;
}