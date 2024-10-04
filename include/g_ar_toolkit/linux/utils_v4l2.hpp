#ifdef __linux__

#ifndef G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_
#define G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_

#include <string>
#include <cstring>

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
            scoped_mmap_buffer(size_t , int , int , int, off_t);
            ~scoped_mmap_buffer();
            operator void*() const;
            operator uint8_t*() const;
            uint8_t * get();
            size_t size();
        private:
            void* m_ptr;
            size_t m_length;
        };

        int xioctl(int, int, void *);
        
        template<typename T>
        void clear_struct(T* ptr){
            std::memset(ptr, 0, sizeof(T));
        }
    }
}

#endif // G_AR_TK__CAPTURE_LV_UTILSV4L2_HPP_
#endif // __linux__