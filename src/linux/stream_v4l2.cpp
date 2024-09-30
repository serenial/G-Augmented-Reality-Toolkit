#include <string>
#include <algorithm>

#include "v4l2_list_devices/list_devices.hpp"

#include "g_ar_toolkit/linux/stream_v4l2.hpp"
#include "g_ar_toolkit/linux/context_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Stream *capture::create_platform_stream(std::string_view device_id, stream_type_t stream_type)
{
    return new StreamV4L2(device_id, stream_type);
}

StreamV4L2::~StreamV4L2(){

}

StreamV4L2::StreamV4L2(std::string_view device_id, stream_type_t stream_type)
    : Stream()
{
    // lookup the device path
    std::vector<v4l2::devices::DEVICE_INFO> device_list;
    v4l2::devices::list(device_list);

    auto device_match = std::find_if(device_list.begin(), device_list.end(), [&](v4l2::devices::DEVICE_INFO device)
                                     { return device_id == device.bus_info && device.device_paths.size() > 0; });

    if (device_match == device_list.end())
    {
        throw std::invalid_argument("Unable to find a suitable matching device with device-id:\"" + std::string(device_id) + "\".");
    }

    std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> supported_formats;

    for (auto const &path : device_match->device_paths)
    {
        // remove any non-matching formats
        supported_formats.erase(std::remove_if(supported_formats.begin(), supported_formats.end(), [&](const std::pair<v4l2_frmivalenum, v4l2_fmtdesc> &item)
                                               {
            bool match = stream_type.height == item.first.height 
            && stream_type.width == item.first.width
            && stream_type.fps_numerator == item.first.discrete.denominator // frame interval so denominator => numerator
            && item.first.discrete.numerator == 1;

            return !match; }),
                                supported_formats.end());

        if (!supported_formats.empty())
        {
            break;
        }
    }

    
}

void StreamV4L2::start_stream()
{
}

void StreamV4L2::stop_stream()
{
}

void StreamV4L2::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
}

// example?
// https://www.marcusfolkesson.se/blog/capture-a-picture-with-v4l2/