#ifdef __linux__

#ifndef G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_

#include <chrono>
#include <utility>
#include <unordered_map>

#include "../capture/context.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        int xioctl(int, int, void *);
        void lookup_support_formats_by_device_path(std::string_view, std::vector<std::pair<v4l2_frmivalenum, v4l2_fmtdesc>> &);

        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {
        public:
            ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices);
        };
    };
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#endif // __linux__
