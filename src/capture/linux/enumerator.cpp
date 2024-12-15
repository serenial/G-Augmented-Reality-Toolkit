#include "v4l2_list_devices/list_devices.hpp"
#include "g_ar_toolkit/capture/enumerator.hpp"
#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/linux/utils.hpp"
#include "g_ar_toolkit/capture/linux/decoders.hpp"

using namespace g_ar_toolkit;
using namespace capture;


void Enumerator::enumerate_devices(std::vector<device_info_t> &devices)
{
    std::vector<v4l2::devices::DEVICE_INFO> device_list;

    v4l2::devices::list(device_list);

    // remove any devices which don't support streaming
    device_list.erase(std::remove_if(device_list.begin(), device_list.end(), [](v4l2::devices::DEVICE_INFO &device)
                                     { capture::remove_device_paths_without_streaming_support(device.device_paths);
                                     return device.device_paths.empty(); }),
                      device_list.end());

    // enumerate supported formats for each device
    for (const auto &device : device_list)
    {
        device_info_t d;
        d.device_id = device.bus_info;
        d.device_name = device.device_description;

        std::vector<Stream::stream_type_t> supported_formats;

        for (auto const &path : device.device_paths)
        {
            std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> v4l2_supported_formats;

            lookup_supported_formats_by_device_path(path, v4l2_supported_formats);

            for (const auto &v4l2_format : v4l2_supported_formats)
            {
                Stream::stream_type_t supported_format;
                supported_format.width = v4l2_format.first.width;
                supported_format.height = v4l2_format.first.height;
                // invert frame-intervals to get frame rate
                supported_format.fps_numerator = v4l2_format.first.discrete.denominator;
                supported_format.fps_denominator = v4l2_format.first.discrete.numerator;

                supported_formats.push_back(supported_format);
            }

            if (supported_formats.size() > 0)
            {
                break;
            }
        }
        d.supported_formats = supported_formats;
        devices.push_back(d);
    }
}

void capture::lookup_supported_formats_by_device_path(std::string_view path, std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> &v4l2_supported_formats)
{
    scoped_file_descriptor s_fd{path, O_RDONLY};
    // Try and open device to test access
    if (s_fd != -1)
    {
        struct v4l2_fmtdesc current_format;
        struct v4l2_frmsizeenum current_size;
        struct v4l2_frmivalenum current_interval;

        current_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        current_format.index = 0;
        for (current_format.index = 0;
             xioctl(
                 s_fd, VIDIOC_ENUM_FMT, &current_format) == 0;
             ++current_format.index)
        {
            // check format is one of the supported ones
            if (!decoder::decoder_available(current_format.pixelformat))
            {
                continue;
            }

            current_size.index = 0;
            current_size.pixel_format = current_format.pixelformat;

            for (current_size.index = 0;
                 xioctl(
                     s_fd, VIDIOC_ENUM_FRAMESIZES, &current_size) == 0;
                 ++current_size.index)
            {
                current_interval.index = 0;
                current_interval.pixel_format = current_size.pixel_format;
                current_interval.width = current_size.discrete.width;
                current_interval.height = current_size.discrete.height;
                for (current_interval.index = 0;
                     xioctl(
                         s_fd, VIDIOC_ENUM_FRAMEINTERVALS, &current_interval) == 0;
                     ++current_interval.index)
                {
                    if (current_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE)
                    {
                        v4l2_supported_formats.emplace_back(current_interval, current_format);
                    }
                } // interval loop
            } // size loop
        } // fmt loop
    }
}

void capture::remove_device_paths_without_streaming_support(std::vector<std::string> &paths)
{
    paths.erase(std::remove_if(paths.begin(), paths.end(), [](const std::string &path)
                               {
                                   scoped_file_descriptor s_fd{path, O_RDONLY};
                                   if (s_fd == -1)
                                   {
                                       return true;
                                   }

                                   struct v4l2_capability cap;
                                   if (xioctl(s_fd, VIDIOC_QUERYCAP, &cap) == -1)
                                   {
                                       return true;
                                   }

                                   return !(cap.capabilities & V4L2_CAP_STREAMING);
                               }));
}