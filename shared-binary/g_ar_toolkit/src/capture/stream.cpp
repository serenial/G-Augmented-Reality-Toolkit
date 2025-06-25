#include <string>
#include <cstdio>
#include <vector>

#include "g_ar_toolkit/capture/stream.hpp"

namespace
{
    static void replace_token_in_string(std::string &str, std::string_view token, std::string_view to_insert)
    {
        std::size_t pos = str.find(token);
        while (pos != std::string::npos)
        {
            str.replace(pos, token.length(), to_insert);
            pos = str.find(token);
        }
    }
}

using namespace g_ar_toolkit;
using namespace capture;

Stream::param_error::param_error(camera_parameters param, std::string_view formatting_string) : std::runtime_error(Stream::param_error::format_message(param, formatting_string)) {}

std::string Stream::param_error::format_message(Stream::camera_parameters param, std::string_view formatting_string)
{
    std::string parameter_name;
    switch (param)
    {
    case Stream::camera_parameters::EXPOSURE:
        parameter_name = "Exposure";
        break;
    case Stream::camera_parameters::FOCUS:
        parameter_name = "Focus";
        break;
    case Stream::camera_parameters::ZOOM:
        parameter_name = "Zoom";
        break;
    case Stream::camera_parameters::WHITE_BALANCE_TEMPERATURE:
        parameter_name = "White Balance";
        break;
    case Stream::camera_parameters::BRIGHTNESS:
        parameter_name = "Brighness";
        break;
    case Stream::camera_parameters::CONTRAST:
        parameter_name = "Contrast";
        break;
    case Stream::camera_parameters::SATURATION:
        parameter_name = "Saturation";
        break;
    case Stream::camera_parameters::GAMMA:
        parameter_name = "Gama";
        break;
    case Stream::camera_parameters::HUE:
        parameter_name = "Hue";
        break;
    case Stream::camera_parameters::SHARPNESS:
        parameter_name = "Sharpness";
        break;
    case Stream::camera_parameters::BACKLIGHT_COMPENSATION:
        parameter_name = "Backlight Compensation";
        break;
    case Stream::camera_parameters::POWER_LINE_FREQUENCY:
        parameter_name = "Power Line Frequency";
        break;
    case Stream::camera_parameters::GAIN:
        parameter_name = "Gain";
        break;
    }

    std::string output{formatting_string};
    replace_token_in_string(output, "%s", parameter_name);
    return output;
}


Stream::auto_param_error::auto_param_error(camera_auto_parameters param, std::string_view formatting_string) : std::runtime_error(Stream::auto_param_error::format_message(param, formatting_string)) {}

std::string Stream::auto_param_error::format_message(Stream::camera_auto_parameters param, std::string_view formatting_string)
{
    std::string parameter_name;
    switch (param)
    {
        case Stream::camera_auto_parameters::AUTO_EXPOSURE:
        parameter_name = "Auto Exposure";        
        break;
    case Stream::camera_auto_parameters::AUTO_FOCUS:
        parameter_name = "Auto Focus";
        break;
    case Stream::camera_auto_parameters::AUTO_WHITE_BALANCE:
        parameter_name = "Auto White Balance";
        break;
    case Stream::camera_auto_parameters::AUTO_GAIN:
        parameter_name = "Auto Gain";
        break;
    }

    std::string output{formatting_string};
    replace_token_in_string(output, "%s", parameter_name);
    return output;
}

// optional default initializer
Stream::param_info_t::param_info_t(): is_supported(false), min(0), max(0), step(0), default_value(-1){

}

Stream::stream_type_t::stream_type_t(uint32_t width, uint32_t height, uint32_t fps_num, uint32_t fps_denom) : 
                        width(width), height(height), fps_numerator(fps_num), fps_denominator(fps_denom){}