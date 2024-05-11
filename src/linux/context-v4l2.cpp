#include "g_ar_toolkit/linux/context-v4l2.hpp"

#include "list_devices.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Context *capture::create_platform_context()
{
    return new ContextV4L2;
}

ContextV4L2::ContextV4L2() : Context()
{
    // init
}

void ContextV4L2::enumerate_devices(std::vector<device_info_t> &devices)
{
    std::vector<v4l2::devices::DEVICE_INFO> device_list;

    v4l2::devices::list(device_list);

    for (const auto & device : device_list) 
    {
        device_info_t d;
        d.device_id = device.device_paths.front();
        d.device_name = device.device_description;
        devices.push_back(d);
    }
}

ContextV4L2::~ContextV4L2()
{
    // de-init
}