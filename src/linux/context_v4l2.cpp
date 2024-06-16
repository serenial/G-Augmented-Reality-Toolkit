#include <unordered_map>

#include "v4l2_list_devices/list_devices.hpp"
#include "g_ar_toolkit/linux/context_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;
     

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

    for (const auto &device : device_list)
    {
        device_info_t d;
        d.device_id = device.bus_info;
        d.device_name = device.device_description;

        std::vector<stream_type_t> supported_formats;

        for (auto const &path : device.device_paths)
        {
            std::vector<std::pair<v4l2_frmivalenum,v4l2_fmtdesc>> v4l2_supported_formats;

            lookup_support_formats_by_device_path(path, v4l2_supported_formats);

            for (const auto &v4l2_format : v4l2_supported_formats)
            {
                stream_type_t supported_format;
                supported_format.width = v4l2_format.first.width;
                supported_format.height = v4l2_format.first.height;
                // invert frame-intervals to get frame rate
                supported_format.fps_numerator = v4l2_format.first.discrete.denominator;
                supported_format.fps_denominator = v4l2_format.first.discrete.numerator;

                supported_formats.push_back(supported_format);
            }

            if (supported_formats.size() > 0)
            {
                break;
            }
        }
        d.supported_formats = supported_formats;
        devices.push_back(d);
    }
}

void capture::lookup_support_formats_by_device_path(std::string_view path, std::vector<std::pair<v4l2_frmivalenum,v4l2_fmtdesc>> &v4l2_supported_formats)
{
    int fd;
    // Try and open device to test access
    if ((fd = open(std::string(path).c_str(), O_RDONLY)) != -1)
    {
        struct v4l2_fmtdesc current_format;
        struct v4l2_frmsizeenum current_size;
        struct v4l2_frmivalenum current_interval;

        current_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        current_format.index = 0;
        for (current_format.index = 0;
             xioctl(
                 fd, VIDIOC_ENUM_FMT, &current_format) == 0;
             ++current_format.index)
        {
            current_size.index = 0;
            current_size.pixel_format = current_format.pixelformat;

            for (current_size.index = 0;
                 xioctl(
                     fd, VIDIOC_ENUM_FRAMESIZES, &current_size) == 0;
                 ++current_size.index)
            {
                current_interval.index = 0;
                current_interval.pixel_format = current_size.pixel_format;
                current_interval.width = current_size.discrete.width;
                current_interval.height = current_size.discrete.height;
                for (current_interval.index = 0;
                     xioctl(
                         fd, VIDIOC_ENUM_FRAMEINTERVALS, &current_interval) == 0;
                     ++current_interval.index)
                {
                    if (current_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE)
                    {
                        v4l2_supported_formats.emplace_back(current_interval,current_format);
                    }
                } // interval loop
            } // size loop
        } // fmt loop
        close(fd);
    }
}

namespace {

    std::unordered_map<__u32, rgb_format_info_t> supported_rgb_formats = {
        {V4L2_PIX_FMT_BGR24, {[](const cv::Mat& in, cv::Mat& out){ cv::cvtColor(in,out,cv::COLOR_BGR2BGRA);}, CV_8UC3}},
        {V4L2_PIX_FMT_RGB24,{[](const cv::Mat& in, cv::Mat& out){ cv::cvtColor(in,out,cv::COLOR_RGB2BGRA);}, CV_8UC3}}, 
        {V4L2_PIX_FMT_ABGR32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 0, 2, 1, 3, 2}), 4);}, CV_8UC4}},
        {V4L2_PIX_FMT_XBGR32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({1, 0, 2, 1, 3, 2}), 3);}, CV_8UC4}}, 
        {V4L2_PIX_FMT_BGRA32,{[](const cv::Mat& in, cv::Mat& out){ in.copyTo(out);}, CV_8UC4}}, 
        {V4L2_PIX_FMT_BGRX32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({0, 0, 1, 1, 2, 2}), 3);}, CV_8UC4}}, 
        {V4L2_PIX_FMT_RGBA32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({0, 2, 1, 1, 2, 0, 3, 3}), 4);}, CV_8UC4}}, 
        {V4L2_PIX_FMT_RGBX32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 2, 2, 1}), 3);}, CV_8UC4}}, 
        {V4L2_PIX_FMT_ARGB32,{[](const cv::Mat& in, cv::Mat& out){ cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 2, 2, 1, 3, 0}), 4);}, CV_8UC4}}
    };

    std::unordered_map<__u32, yuv_format_info_t> supported_yuv_formats = {
        {V4L2_PIX_FMT_NV12, {[](std::pair<const cv::Mat&,const cv::Mat&> planes, cv::Mat& out){cv::cvtColorTwoPlane(planes.first,planes.second,out, cv::COLOR_YUV2BGRA_NV12);}}},
        {V4L2_PIX_FMT_NV21, {[](std::pair<const cv::Mat&,const cv::Mat&> planes, cv::Mat& out){cv::cvtColorTwoPlane(planes.first,planes.second,out, cv::COLOR_YUV2BGRA_NV21);}}}
    };

        /*
          cv::COLOR_YUV2BGRA_YV12 = 103,
            cv::COLOR_YUV2BGRA_IYUV = 105,
              cv::COLOR_YUV2BGRA_I420
               cv::COLOR_YUV2BGRA_UYVY = 112,
                 cv::COLOR_YUV2BGRA_Y422
                   cv::COLOR_YUV2BGRA_Y422 =
                   cv::COLOR_YUV2BGRA_UYNV
                   cv::COLOR_YUV2BGRA_YUY2 
                     cv::COLOR_YUV2BGRA_YVYU = 122,
                      cv::COLOR_YUV2BGRA_YUYV
                        cv::COLOR_YUV2BGRA_YUNV 
        
        
        
        
        
        */
}

int capture::xioctl(int fh, int request, void *arg)
{
    int r;

    do 
    {
        r = ioctl(fh, request, arg);
    } while ((r == -1) && (errno == EINTR));

    return r;
}

std::optional<rgb_format_info_t> capture::lookup_rgb_format(__u32 format){
    auto f = supported_rgb_formats.find(format);
    return f == supported_rgb_formats.end()? std::nullopt : std::optional(f->second);
}

std::optional<yuv_format_info_t> capture::lookup_yuv_format(__u32 format){
    auto f = supported_yuv_formats.find(format);
    return f == supported_yuv_formats.end()? std::nullopt : std::optional(f->second);
}  