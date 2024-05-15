#include "list_devices.hpp"
#include "usb_cam/utils.hpp"

#include "g_ar_toolkit/linux/context-v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

stream_pixel_format fourcc_to_stream_pixel_format(__u32);

Context *capture::create_platform_context()
{
    return new ContextV4L2;
}

ContextV4L2::ContextV4L2() : Context()
{
    // init
}

void ContextV4L2::enumerate_devices(std::vector<device_info_t> &devices)
{
    std::vector<v4l2::devices::DEVICE_INFO> device_list;

    v4l2::devices::list(device_list);

    for (const auto &device : device_list)
    {
        device_info_t d;
        d.device_id = device.bus_info;
        d.device_name = device.device_description;

        std::vector<stream_type_t> supported_formats;

        for (auto const &path : device.device_paths)
        {
            int fd;
            // Try and open device to test access
            if ((fd = open(path.c_str(), O_RDONLY)) != -1)
            {
                struct v4l2_fmtdesc current_format;
                struct v4l2_frmsizeenum current_size;
                struct v4l2_frmivalenum current_interval;

                current_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                current_format.index = 0;
                for (current_format.index = 0;
                     usb_cam::utils::xioctl(
                         fd, VIDIOC_ENUM_FMT, &current_format) == 0;
                     ++current_format.index)
                {
                    current_size.index = 0;
                    current_size.pixel_format = current_format.pixelformat;

                    for (current_size.index = 0;
                         usb_cam::utils::xioctl(
                             fd, VIDIOC_ENUM_FRAMESIZES, &current_size) == 0;
                         ++current_size.index)
                    {
                        current_interval.index = 0;
                        current_interval.pixel_format = current_size.pixel_format;
                        current_interval.width = current_size.discrete.width;
                        current_interval.height = current_size.discrete.height;
                        for (current_interval.index = 0;
                             usb_cam::utils::xioctl(
                                 fd, VIDIOC_ENUM_FRAMEINTERVALS, &current_interval) == 0;
                             ++current_interval.index)
                        {
                            if (current_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE)
                            {

                                stream_type_t supported_format;
                                supported_format.width = current_interval.width;
                                supported_format.height = current_interval.height;
                                // invert frame-intervals to get frame rate
                                supported_format.fps.numerator = current_interval.discrete.denominator;
                                supported_format.fps.denominator = current_interval.discrete.numerator;
                                supported_format.format = fourcc_to_stream_pixel_format(current_format.pixelformat);
                                supported_formats.push_back(supported_format);
                            }
                        } // interval loop
                    } // size loop
                } // fmt loop
                close(fd);
                break;
            }
        }
        d.supported_formats = supported_formats;
        devices.push_back(d);
    }
}

ContextV4L2::~ContextV4L2()
{
    // de-init
}

stream_pixel_format fourcc_to_stream_pixel_format(__u32 pixel_format)
{
    switch (pixel_format)
    {
    case V4L2_PIX_FMT_RGB24:
        return stream_pixel_format::RGB24;
    case V4L2_PIX_FMT_RGB32:
        return stream_pixel_format::RGB32;
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_YYUV:
    case V4L2_PIX_FMT_UYVY:
    case V4L2_PIX_FMT_VYUY:
        return stream_pixel_format::YUV;
    case V4L2_PIX_FMT_NV12:
        return stream_pixel_format::NV12;
    case V4L2_PIX_FMT_MJPEG:
        return stream_pixel_format::MJPEG;
    case V4L2_PIX_FMT_H264:
        return stream_pixel_format::H264;
    }
    return stream_pixel_format::UNKNOWN;
}