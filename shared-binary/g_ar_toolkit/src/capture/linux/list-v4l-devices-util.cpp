#include <vector>
#include <iostream>


#include "g_ar_toolkit/capture/enumerator.hpp"
#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/linux/utils.hpp"
#include "g_ar_toolkit/capture/linux/decoders.hpp"

using namespace g_ar_toolkit;
using namespace capture;

int main(int argc, char const *argv[]){

    std::vector<v4l2_device_t> device_list;
    list_v4l2_devices(device_list);

    std::cout << "Count: " << device_list.size() << std::endl;
    std::cout << "===================================" << std::endl;
    
    for(const auto &dev: device_list){

        std::cout << "- " << dev.device_id << std::endl;
        std::cout << "- " << dev.device_name << std::endl;
        std::cout << "- " << dev.path << std::endl << std::endl;

        for(const auto &format: dev.format_info){
            std::cout << format.first.width << " x " << format.first.height << std::endl;
            std::cout << format.second.description << std::endl << std::endl;
        }


        std::cout << "===================================" << std::endl;

    }

}