#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <filesystem>
#include <algorithm>

#include "g_ar_toolkit/capture/linux/utils.hpp"
#include "g_ar_toolkit/capture/linux/decoders.hpp"

using namespace g_ar_toolkit;
using namespace capture;

int capture::xioctl(int fh, int request, void *arg)
{

    int r;

    while (1)
    {
        errno = 0;
        r = ioctl(fh, request, arg);

        if (r != -1)
        {
            // success!
            break;
        }

        switch (errno)
        {
        case EINTR:
        case EAGAIN:
            break; // continue looping
        case EBUSY:
        case EPERM:
        default:
            return r; // failed
        }
    }
    return r;
}

scoped_file_descriptor::scoped_file_descriptor(std::string_view path, int flags) : m_fd(open(std::string(path).c_str(), flags)) {};

scoped_file_descriptor::~scoped_file_descriptor()
{
    if (m_fd != -1)
    {
        close(m_fd);
        m_fd = -1;
    }
}

scoped_file_descriptor::scoped_file_descriptor(scoped_file_descriptor &&other) : m_fd(other.m_fd)
{
    other.m_fd = -1;
}

scoped_file_descriptor::operator int() const { return m_fd; }

scoped_mmap_buffer::scoped_mmap_buffer(scoped_mmap_buffer &&other)
{
    m_ptr = other.m_ptr;
    m_length = other.m_length;
    other.m_ptr = MAP_FAILED;
    other.m_length = 0;
}

scoped_mmap_buffer::scoped_mmap_buffer(size_t length, int prot, int flags, int fd, off_t offset)
    : m_ptr(reinterpret_cast<uint8_t *>(mmap(nullptr, length, prot, flags, fd, offset))), m_length(m_ptr == MAP_FAILED ? 0 : length)
{
}

scoped_mmap_buffer::~scoped_mmap_buffer()
{
    if (m_ptr != MAP_FAILED)
    {
        munmap(m_ptr, m_length);
        m_ptr = MAP_FAILED;
        m_length = 0;
    }
}

scoped_mmap_buffer::operator void *() const { return m_ptr; }
scoped_mmap_buffer::operator uint8_t *() const { return reinterpret_cast<uint8_t *>(m_ptr); }

v4l2_device_t::v4l2_device_t(const std::string &path, const std::string &device_id, const std::string &device_name, const scoped_file_descriptor &s_fd)
    : path(path), device_id(device_id), device_name(device_name)
{
    // build a list of the supported formats
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
                        format_info.emplace_back(current_interval, current_format);
                    }
                } // interval loop
            } // size loop
        } // fmt loop
    }
}

void capture::list_v4l2_devices(std::vector<v4l2_device_t> &device_list)
{
    // list /dev/ directory
    for (const auto &entry : std::filesystem::directory_iterator{"/dev"})
    {

        if (entry.path().filename().string().find("video") == 0)
        {
            // filename begins with "video" so assume it is a video device

            scoped_file_descriptor s_fd{entry.path().string(), O_RDONLY};

            if (s_fd != -1)
            {
                // is a valid file

                v4l2_capability dev_capability;

                if (xioctl(s_fd, VIDIOC_QUERYCAP, &dev_capability) == -1 &&
                    (dev_capability.device_caps & V4L2_CAP_VIDEO_CAPTURE != 0) && // can capture
                    (dev_capability.device_caps & V4L2_CAP_READWRITE != 0) &&     // can read/write
                    (dev_capability.device_caps & V4L2_CAP_STREAMING != 0)        // can stream
                )
                {
                    // not able to query capabilites or not supported
                    continue;
                }

                v4l2_device_t dev{
                    entry.path().string(),
                    reinterpret_cast<const char *>(dev_capability.bus_info),
                    reinterpret_cast<const char *>(dev_capability.card),
                    s_fd
                };

                if(dev.format_info.empty()){
                    // no supported fomats
                    continue;
                }

                device_list.push_back(dev);
            }
        }
    }
}
