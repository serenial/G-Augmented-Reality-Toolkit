#include <string>
#include <algorithm>
#include <utility>
#include <vector>

#include <sys/mman.h>

#include "v4l2_list_devices/list_devices.hpp"

#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/linux/utils.hpp"
#include "g_ar_toolkit/capture/linux/decoders.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

namespace
{
    std::pair<scoped_file_descriptor, __u32> open_and_configure_device_file_descriptor(std::string_view device_id, Stream::stream_type_t stream_type)
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

    std::vector<scoped_mmap_buffer> create_buffer_list(int fd, std::string_view device_id)
    {

        std::vector<scoped_mmap_buffer> buffer_list;

        struct v4l2_requestbuffers req;
        clear_struct(&req);
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        int count = 1;

        // try to find minimum number of buffers required starting with 2;
        do
        {
            req.count = ++count;
            if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1)
            {
                throw std::runtime_error("Unable to query buffer requirements.");
            }
        } while (req.count != count && count < 128);

        if (count != req.count)
        {
            throw std::runtime_error("Number of buffers exceeds maximum expected number.");
        }

        // reserve the buffer list length;
        buffer_list.reserve(req.count);

        for (int i = 0; i < req.count; i++)
        {
            struct v4l2_buffer buf;
            clear_struct(&buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
            {
                throw std::runtime_error("Unable to allocate buffers.");
            }

            // create an mmap buffer - emplace it into the buffer_list
            buffer_list.emplace_back(buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        }

        return buffer_list;
    }
}

Stream::~Stream()
{
    stop_stream();
}

// call delegated constructor
Stream::Stream(std::string_view device_id, stream_type_t stream_type) : Stream(device_id, stream_type, open_and_configure_device_file_descriptor(device_id, stream_type))
{
}

// private constructor that actually does the constructing
Stream::Stream(std::string_view device_id, stream_type_t stream_type, std::pair<scoped_file_descriptor, __u32> fd)
    : m_stream_type(stream_type), m_device_id(device_id), m_scoped_fd(std::move(fd.first)),
      m_pixel_format(fd.second), m_decoder(decoder::create(fd.second, stream_type.width, stream_type.height)),
      m_buffer_list(create_buffer_list(m_scoped_fd, device_id)), m_is_streaming(false)
{
}

void Stream::start_stream()
{
    if (m_is_streaming)
    {
        return;
    }

    for (int i = 0; i < m_buffer_list.size(); i++)
    {
        enqueue_buffer(i);
    }

    // Start the stream
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(m_scoped_fd, VIDIOC_STREAMON, &type) == -1)
    {
        throw std::runtime_error("Unable to start stream.");
    }

    m_is_streaming = true;
}

void Stream::stop_stream()
{
    if (!m_is_streaming)
    {
        return;
    }

    // Start the stream
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(m_scoped_fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        throw std::runtime_error("Unable to stop stream.");
    }

    m_is_streaming = false;
}

void Stream::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
    if (!m_is_streaming)
    {
        throw std::invalid_argument("Stream has not been started.");
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = begin + timeout;

    while (std::chrono::steady_clock::now() < end)
    {
        // try and dequeue buffer
        size_t bytes;
        int index = dequeue_buffer(&bytes);
        if (index >= 0 && index < m_buffer_list.size())
        {
            // buffer dequeued OK
            m_decoder->decode(m_buffer_list[index], destination, bytes);
            // enqueue the buffer so it can be reused
            enqueue_buffer(index);
            // escape the loop
            break;
        }
    }
}

int Stream::dequeue_buffer(size_t *n_bytes)
{
    struct v4l2_buffer bufd;
    clear_struct(&bufd);

    bufd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufd.memory = V4L2_MEMORY_MMAP;
    bufd.index = 0;

    if (xioctl(m_scoped_fd, VIDIOC_DQBUF, &bufd) == -1)
    {
        if (errno == EAGAIN)
        {
            return -1;
        }
        throw std::runtime_error("Unable to dequeue buffer.");
    }

    if (n_bytes)
    {
        *n_bytes = bufd.bytesused;
    }

    return bufd.index;
}

void Stream::enqueue_buffer(int index)
{

    struct v4l2_buffer bufd;
    clear_struct(&bufd);

    bufd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufd.memory = V4L2_MEMORY_MMAP;
    bufd.index = index;

    if (xioctl(m_scoped_fd, VIDIOC_QBUF, &bufd) == -1)
    {
        throw std::runtime_error("Unable to enqueue buffer.");
    }
}

void Stream::get_camera_parameter_info(Stream::camera_parameters param, Stream::param_info_t* info)
{

    v4l2_queryctrl ctrl;

    set_query_struct_id_by_camera_parameter(ctrl, param);

    if (xioctl(m_scoped_fd, VIDIOC_QUERYCTRL, &ctrl) == -1)
    {
        info->is_supported = false;
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }

    info->default_value = ctrl.default_value;
    info->max = ctrl.maximum;
    info->min = ctrl.minimum;
    info->step = ctrl.step;
    info->is_supported = true;

    // check we can get and set

    set_camera_parameter(param,get_camera_parameter(param));
}

int32_t Stream::get_camera_parameter(Stream::camera_parameters param)
{

    v4l2_control ctrl;

    set_query_struct_id_by_camera_parameter(ctrl, param);

    if (xioctl(m_scoped_fd, VIDIOC_G_CTRL, &ctrl) == -1)
    {
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }

    return ctrl.value;
}

void Stream::set_camera_parameter(Stream::camera_parameters param, int32_t value)
{

    v4l2_control ctrl;

    set_query_struct_id_by_camera_parameter(ctrl, param);

    ctrl.value = value;
    if (xioctl(m_scoped_fd, VIDIOC_S_CTRL, &ctrl) == -1)
    {
        throw Stream::param_error{param, "Unable to set camera parameter %s."};
    }
}

bool Stream::get_camera_auto_mode(Stream::camera_auto_parameters param)
{
    v4l2_control ctrl;
    set_query_struct_id_by_auto_camera_parameter(ctrl, param);

    if (xioctl(m_scoped_fd, VIDIOC_G_CTRL, &ctrl) == -1)
    {
        throw Stream::auto_param_error(param, "Unable to get the auto-camera parameter %s.");
    }

    // handle Exposure being a non-boolean
    if (ctrl.id == V4L2_CID_EXPOSURE_AUTO)
    {
        return (ctrl.value == V4L2_EXPOSURE_MANUAL) ? 0 : 1;
    }
    else
    {
        return (ctrl.value != 0);
    }
    return false;
}

void Stream::set_camera_auto_mode(Stream::camera_auto_parameters param, bool enabled)
{
        v4l2_control ctrl;
        set_query_struct_id_by_auto_camera_parameter(ctrl, param);

        if(param == Stream::camera_auto_parameters::AUTO_EXPOSURE){
            ctrl.value = enabled ? V4L2_EXPOSURE_APERTURE_PRIORITY : V4L2_EXPOSURE_MANUAL;
        }
        else{
            ctrl.value = enabled? 1:0;
        }

    if (xioctl(m_scoped_fd, VIDIOC_S_CTRL, &ctrl)==-1)
    {
        throw Stream::auto_param_error(param, "Unable to set the auto-camera parameter %s.");  
    }  
}