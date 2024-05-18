#include <string>
#include <algorithm>
#include "list_devices.hpp"

#include "g_ar_toolkit/linux/stream-v4l2.hpp"
#include "g_ar_toolkit/linux/context-v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Stream* capture::create_platform_stream(std::string device_id, stream_type_t stream_type)
{
    return new StreamV4L2(device_id, stream_type);
}

StreamV4L2::StreamV4L2(std::string device_id, stream_type_t stream_type) : Stream(), usb_cam_ptr(std::make_unique<usb_cam::UsbCam>()){
    // lookup the device path
    std::vector<v4l2::devices::DEVICE_INFO> device_list;
    v4l2::devices::list(device_list);

    auto device_match = std::find_if(device_list.begin(), device_list.end(),[&](v4l2::devices::DEVICE_INFO device){return device_id==device.bus_info && device.device_paths.size()>0;});

    if(device_match == device_list.end()){
        throw std::invalid_argument("Unable to find a suitable matching device with device-id:\"" + device_id + "\".");
    }

    // set usb_cam_parameters
    usb_cam_parameters.camera_name = device_id;
    usb_cam_parameters.device_name = device_match->device_paths.front();
    usb_cam_parameters.image_height = stream_type.height;
    usb_cam_parameters.image_width = stream_type.width;
    usb_cam_parameters.framerate = stream_type.fps.numerator;

    // lookup/guess suitable format:

}

StreamV4L2::~StreamV4L2(){
    // deinit
}


void StreamV4L2::start_stream(){}
void StreamV4L2::stop_stream(){}
void StreamV4L2::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout){}