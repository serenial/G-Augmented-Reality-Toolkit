#include "g_ar_toolkit/capture/enumerator.hpp"
#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/linux/utils.hpp"
#include "g_ar_toolkit/capture/linux/decoders.hpp"

using namespace g_ar_toolkit;
using namespace capture;

void Enumerator::enumerate_devices(std::vector<device_info_t> &devices)
{
    std::vector<v4l2_device_t> device_list;
    list_v4l2_devices(device_list);

    std::transform(
        device_list.begin(),
        device_list.end(),
        std::back_inserter(devices),
        [](const v4l2_device_t &v)
        {
            device_info_t d;

            d.device_id = v.device_id;
            d.device_name = v.device_name;

            std::transform(
                v.format_info.begin(),
                v.format_info.end(),
                std::back_inserter(d.supported_formats),
                [](const auto &f)
                {
                    return Stream::stream_type_t(
                        f.first.width,
                        f.first.height,
                        f.first.discrete.denominator, // v4l2 uses frame interval so inverse of FPS
                        f.first.discrete.numerator);
                });

            return d;
        });
}