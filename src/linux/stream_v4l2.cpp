#include <string>
#include <algorithm>

#include "v4l2_list_devices/list_devices.hpp"

#include "g_ar_toolkit/linux/stream_v4l2.hpp"
#include "g_ar_toolkit/linux/context_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Stream *capture::create_platform_stream(std::string device_id, stream_type_t stream_type, uint32_t options)
{
    return new StreamV4L2(device_id, stream_type, options);
}

StreamV4L2::StreamV4L2(std::string device_id, stream_type_t stream_type, uint32_t options) : Stream(), usb_cam_ptr(std::make_unique<usb_cam::UsbCam>())
{
    // lookup the device path
    std::vector<v4l2::devices::DEVICE_INFO> device_list;
    v4l2::devices::list(device_list);

    auto device_match = std::find_if(device_list.begin(), device_list.end(), [&](v4l2::devices::DEVICE_INFO device)
                                     { return device_id == device.bus_info && device.device_paths.size() > 0; });

    if (device_match == device_list.end())
    {
        throw std::invalid_argument("Unable to find a suitable matching device with device-id:\"" + device_id + "\".");
    }

    // set usb_cam_parameters
    usb_cam_parameters.camera_name = device_id;
    usb_cam_parameters.image_height = stream_type.height;
    usb_cam_parameters.image_width = stream_type.width;
    usb_cam_parameters.framerate = stream_type.fps_numerator;

    // try to find a suitable format match
    std::vector<std::pair<v4l2_frmivalenum,v4l2_fmtdesc>> supported_formats;

    const std::unordered_map<__u32, format_item_t> format_lookup = get_format_lookup();

    for (auto const &path : device_match->device_paths)
    {
        usb_cam_parameters.device_name = path;

        lookup_support_formats_by_device_path(path, supported_formats);
        uint32_t pixel_format = get_pixel_format_from_options(options);

        // remove any non-matching formats
        supported_formats.erase(std::remove_if(supported_formats.begin(), supported_formats.end(), [&](const std::pair<v4l2_frmivalenum,v4l2_fmtdesc> &item)
                                               {
            bool match = stream_type.height == item.first.height 
            && stream_type.width == item.first.width
            && stream_type.fps_numerator == item.first.discrete.denominator // frame interval so denominator => numerator
            && item.first.discrete.numerator == 1
            // pixel_format == 0 is match_any
            && (pixel_format == 0 || format_lookup.at(item.first.pixel_format).index == pixel_format);
            return !match; }),
                                supported_formats.end());

        if (!supported_formats.empty())
        {
            break;
        }
    }

    if (supported_formats.empty())
    {
        throw std::invalid_argument("Unable to find matching stream format for device with device-id:\"" + device_id + "\".");
    }
    
    // get the format-descritption of the first remaining format
    auto &desc = supported_formats.front().second.description;

    usb_cam_parameters.pixel_format_name = format_lookup.at(supported_formats.front().first.pixel_format).convertor_name;
    usb_cam_parameters.av_device_format = "yuv422p";

    // parse values from options
    options_t opts;
    opts.as_value = options;

    usb_cam::utils::io_method_t io_method = usb_cam::utils::IO_METHOD_MMAP;
    switch (opts.as_bits.io_method)
    {
    case 1:
        io_method = usb_cam::utils::IO_METHOD_READ;
        break;
    case 2:
        io_method = usb_cam::utils::IO_METHOD_USERPTR;
        break;
    }

    usb_cam_ptr->configure(usb_cam_parameters, io_method);
}

StreamV4L2::~StreamV4L2()
{
    // deinit
}

void StreamV4L2::start_stream() {}
void StreamV4L2::stop_stream() {}
void StreamV4L2::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout) {}
