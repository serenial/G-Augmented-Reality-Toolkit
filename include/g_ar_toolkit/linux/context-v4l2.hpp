#ifdef __linux__

#ifndef G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_

#include <chrono>
#include <unordered_map>

#include "../capture/context.hpp"

extern "C"
{
#include <linux/videodev2.h>
}

namespace g_ar_toolkit
{
    namespace capture
    {

        void lookup_support_formats_by_device_path(std::string, std::vector<v4l2_frmivalenum> &);
        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {
            const std::unordered_map<__u32, format_item_t> format_lookup;
        public:
            ContextV4L2();
            ~ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices);
        };
    };
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#endif // __linux__
