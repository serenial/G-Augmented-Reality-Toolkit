#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "g_ar_toolkit/linux/utils_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;

int xioctl(int fh, int request, void *arg)
{
    int r;

    do
    {
        r = ioctl(fh, request, arg);
    } while ((r == -1) && (errno == EINTR));

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
