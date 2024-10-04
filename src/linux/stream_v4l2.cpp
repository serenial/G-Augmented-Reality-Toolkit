#include <string>
#include <algorithm>
#include <utility>

#include "v4l2_list_devices/list_devices.hpp"

#include "g_ar_toolkit/linux/stream_v4l2.hpp"
#include "g_ar_toolkit/linux/context_v4l2.hpp"
#include "g_ar_toolkit/linux/utils_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

namespace
{
    std::pair<scoped_file_descriptor, __u32> open_and_configure_device_file_descriptor(std::string_view device_id, stream_type_t stream_type)
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

        // remove any device paths which don't support streaming
        remove_device_paths_without_streaming_support(device_match->device_paths);

        if (device_match->device_paths.empty())
        {
            throw std::invalid_argument("The device with device-id:\"" + std::string(device_id) + "\" does not support streaming.");
        }

        std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> supported_formats;
        std::string_view matching_path;

        for (auto const &path : device_match->device_paths)
        {
            matching_path = path;
            // get the list of supported formats for this path
            lookup_supported_formats_by_device_path(path, supported_formats);

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
                // valid formats so stop looping
                break;
            }
        }

        if (supported_formats.empty())
        {
            throw std::invalid_argument("Unable to create the stream for device with device-id:\"" + std::string(device_id) + "\" with the dimensions and FPS requested.");
        }

        // try and request the format with any of the supported_formats
        struct v4l2_format format;
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width = stream_type.width;
        format.fmt.pix.height = stream_type.height;
        format.fmt.pix.field = V4L2_FIELD_NONE;

        scoped_file_descriptor s_fd{matching_path, O_RDWR};

        for (const auto &supported_format : supported_formats)
        {
            format.fmt.pix.pixelformat = supported_format.first.pixel_format;

            if (xioctl(s_fd, VIDIOC_S_FMT, &format) != -1)
            {
                // it worked
                return std::make_pair(std::move(s_fd), supported_format.first.pixel_format);
            }
        }

        throw std::runtime_error("Unable to configure the stream for device with device-id:\"" + std::string(device_id) + "\" with the dimensions and FPS requested.");
    }
}

Stream *capture::create_platform_stream(std::string_view device_id, stream_type_t stream_type)
{
    return new StreamV4L2(device_id, stream_type);
}

StreamV4L2::~StreamV4L2()
{
}

// call delegated constructor
StreamV4L2::StreamV4L2(std::string_view device_id, stream_type_t stream_type) : StreamV4L2(device_id, stream_type, open_and_configure_device_file_descriptor(device_id, stream_type))
{
}

// private constructor that actually does the constructing
StreamV4L2::StreamV4L2(std::string_view device_id, stream_type_t stream_type, std::pair<scoped_file_descriptor, __u32> fd)
    : Stream(), m_stream_type(stream_type), m_scoped_fd(std::move(fd.first)), m_pixel_format(fd.second)
{
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