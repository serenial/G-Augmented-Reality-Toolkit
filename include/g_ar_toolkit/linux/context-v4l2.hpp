#pragma once

#ifdef __linux__

#include "usb_cam/usb_cam.hpp"

#include "../capture/context.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class ContextV4L2 : public g_ar_toolkit::capture::Context
        {

        public:
            ContextV4L2();
            ~ContextV4L2();
            void enumerate_devices(std::vector<device_info_t> &devices);
        };
    };
}

#endif