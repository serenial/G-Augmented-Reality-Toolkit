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
            void enumerate_devices(std::vector<device_info_t> &devices);
        };

        struct rgb_format_info_t
        {
            std::function<void(const cv::Mat &, cv::Mat &)> convert_colours;
            int buffer_cv_type;
        };

        struct yuv_format_info_t
        {
            std::function<void(const cv::Mat &, cv::Mat &)> convert_colours;
        };

        struct yuv_interlaced_format_info_t
        {
            std::function<void(std::pair<const cv::Mat &, const cv::Mat &>, cv::Mat &)> convert_planes;
        };

        class CompressedDecoder
        {
        public:
            virtual ~CompressedDecoder() = default;
            virtual void decode(const cv::Mat &, cv::Mat &) = 0;
        };

        struct compressed_format_info_t
        {
            std::function<std::unique_ptr<CompressedDecoder>()> create_decoder;
        };

        int xioctl(int, int, void *);
        void lookup_supported_formats_by_device_path(std::string_view, std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> &);

        // format lookup

        std::optional<rgb_format_info_t> lookup_rgb_format(__u32);
        std::optional<yuv_format_info_t> lookup_yuv_format(__u32);
        std::optional<yuv_interlaced_format_info_t> lookup_yuv_interlaced_format(__u32);
        std::optional<compressed_format_info_t> lookup_compressed_format(__u32);
        bool format_is_supported(__u32);

        class scoped_file_descriptor
        {
        public:
            scoped_file_descriptor() = delete;
            scoped_file_descriptor(const scoped_file_descriptor &) = delete;
            scoped_file_descriptor(std::string_view, int flags);
            ~scoped_file_descriptor();
            operator int() const { return m_fd; }

        private:
            int m_fd;
        };

        std::optional<std::string> find_first_device_path_that_supports_streaming(const std::vector<std::string> &);
    }
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#endif // __linux__
