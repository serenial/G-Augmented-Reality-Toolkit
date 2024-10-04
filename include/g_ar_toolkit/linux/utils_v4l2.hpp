#ifdef __linux__

#ifndef G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_
#define G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_

#include <string>

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
            operator int() const { return m_fd; }

        private:
            int m_fd;
        };

        int xioctl(int, int, void *);
    }
}

#endif // G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_
#endif // __linux__