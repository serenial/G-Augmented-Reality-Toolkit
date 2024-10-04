#ifdef __linux__

#ifndef G_AR_TK__CAPTURE_LV_CONTEXTV4L2_HPP_
#define G_AR_TK__CAPTURE_LV_CONTEXTV4L2_HPP_

#include <linux/videodev2.h>

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

        void remove_device_paths_without_streaming_support(std::vector<std::string>&);
        void lookup_supported_formats_by_device_path(std::string_view, std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> &);
    }
}

#endif // G_AR_TK__CAPTURE_LV_CONTEXTV4L2_HPP_
#endif // __linux__
