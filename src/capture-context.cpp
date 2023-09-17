#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit_export.h"

#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;

Stream* Context::open_stream(std::string device_id, stream_type_t stream_format){
    return create_platform_stream(device_id, stream_format);
}
Context::Context(){}
Context::~Context(){}

void Context::enumerate_devices(std::vector<device_info_t> &devices){}