#ifdef __linux__

#ifndef G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_

#include <utility>
#include <optional>
#include <functional>
#include <memory>

#include <linux/videodev2.h>

#include <opencv2/imgproc.hpp>

#include "../capture/context.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {
        public:
            ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices) override;
        };

        int xioctl(int, int, void *);

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

        void remove_device_paths_without_streaming_support(std::vector<std::string>&);
        void lookup_supported_formats_by_device_path(std::string_view, std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> &);
    }
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#endif // __linux__
