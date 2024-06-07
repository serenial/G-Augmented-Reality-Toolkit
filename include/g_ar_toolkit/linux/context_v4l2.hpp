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

        void lookup_support_formats_by_device_path(std::string_view, std::vector<std::pair<v4l2_frmivalenum,v4l2_fmtdesc>> &);
        const std::unordered_map<__u32, format_item_t> get_format_lookup();

        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {
        const std::unordered_map<__u32, format_item_t> format_lookup;
        public:
            ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices);
            void list_of_formats(std::vector<format_item_t>&);
        };
    };
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTV4L2_HPP_
#endif // __linux__
