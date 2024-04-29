#include "g_ar_toolkit/linux/context-v4l2.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

#include "usb_cam/utils.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Context *capture::create_platform_context()
{
    return new ContextV4L2;
}

ContextV4L2::ContextV4L2() : Context(){
    //init
}

void ContextV4L2::enumerate_devices(std::vector<device_info_t> &devices){
    std::map<std::string, v4l2_capability> x = usb_cam::utils::available_devices();
}

ContextV4L2::~ContextV4L2(){
    //de-init
}