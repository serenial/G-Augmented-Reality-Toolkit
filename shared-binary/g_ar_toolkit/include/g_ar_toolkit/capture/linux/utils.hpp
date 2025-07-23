#pragma once

#include <string>
#include <cstring>
#include <vector>

#include <linux/videodev2.h>

namespace g_ar_toolkit
{
    namespace capture
    {
        class scoped_file_descriptor
        {
        public:
            scoped_file_descriptor() = delete;
            scoped_file_descriptor(const scoped_file_descriptor &) = delete;
            scoped_file_descriptor(scoped_file_descriptor &&);
            scoped_file_descriptor(std::string_view, int flags);
            ~scoped_file_descriptor();
            operator int() const;

        private:
            int m_fd;
        };

        class scoped_mmap_buffer
        {
        public:
            scoped_mmap_buffer() = delete;
            scoped_mmap_buffer(const scoped_mmap_buffer &) = delete;
            scoped_mmap_buffer(scoped_mmap_buffer &&);
            scoped_mmap_buffer(size_t, int, int, int, off_t);
            ~scoped_mmap_buffer();
            operator void *() const;
            operator uint8_t *() const;
            uint8_t *get();
            size_t size();

        private:
            void *m_ptr;
            size_t m_length;
        };

        int xioctl(int, int, void *);

        // used to clear buffer for mmapped streaming
        template <typename T>
        void clear_struct(T *ptr)
        {
            std::memset(ptr, 0, sizeof(T));
        }

        struct v4l2_device_t
        {
            const std::string path;
            const std::string device_id;
            const std::string device_name;
            std::vector<std::pair<const v4l2_frmivalenum, const v4l2_fmtdesc>> format_info;
            v4l2_device_t() = delete;
            v4l2_device_t(const std::string &, const std::string &, const std::string &, const scoped_file_descriptor &s_fd);
        };

        void list_v4l2_devices(std::vector<v4l2_device_t> &);
    }
}