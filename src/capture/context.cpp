#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;

Stream *Context::open_stream(std::string_view device_id, stream_type_t stream_format)
{
    return create_platform_stream(device_id, stream_format);
}