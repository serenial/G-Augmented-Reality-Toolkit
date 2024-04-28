#include <string>

#include "g_ar_toolkit/linux/stream-v4l2.hpp"
#include "g_ar_toolkit/linux/context-v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Stream* capture::create_platform_stream(std::string device_id, stream_type_t stream_type)
{
    return new StreamV4L2(device_id, stream_type);
}

