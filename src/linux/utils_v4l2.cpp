#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


#include "g_ar_toolkit/linux/utils_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;

int capture::xioctl(int fh, int request, void *arg)
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

scoped_file_descriptor::operator int() const { return m_fd; }

scoped_mmap_buffer::scoped_mmap_buffer(scoped_mmap_buffer &&other){
    m_ptr = other.m_ptr;
    m_length = other.m_length;
    other.m_ptr = MAP_FAILED;
    other.m_length = 0;
}

scoped_mmap_buffer::scoped_mmap_buffer(size_t length, int prot, int flags, int fd, off_t offset)
    : m_ptr(reinterpret_cast<uint8_t*>(mmap(nullptr, length, prot, flags, fd, offset))), m_length(m_ptr == MAP_FAILED? 0 : length){
}

scoped_mmap_buffer::~scoped_mmap_buffer(){
    if(m_ptr != MAP_FAILED){
        munmap(m_ptr, m_length);
        m_ptr = MAP_FAILED;
        m_length = 0;
    }
}

scoped_mmap_buffer::operator void*() const { return m_ptr; }
scoped_mmap_buffer::operator uint8_t*() const { return reinterpret_cast<uint8_t*>(m_ptr);}
