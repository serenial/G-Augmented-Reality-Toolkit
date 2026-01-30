#include <unordered_map>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <linux/videodev2.h>
#include "g_ar_toolkit/capture/linux/decoders.hpp"
#include "g_ar_toolkit/capture/linux/decoder-mjpeg.hpp"

using namespace g_ar_toolkit;
using namespace capture;

namespace
{
    std::unordered_map<__u32, std::function<void(const uint8_t *, cv::Mat &, const cv::Size, const size_t data_bytes)>> supported_simple_formats = {
        {V4L2_PIX_FMT_BGR24, [](const uint8_t *data, cv::Mat &out, const cv::Size dims, const size_t data_bytes)
         {
             const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t *>(data)};
             if (out.channels() == 1)
             {
                 cv::cvtColor(in, out, cv::COLOR_BGR2GRAY);
             }
             else
             {
                 cv::cvtColor(in, out, cv::COLOR_BGR2BGRA);
             }
         }},
        {V4L2_PIX_FMT_RGB24, [](const uint8_t *data, cv::Mat &out, const cv::Size dims, const size_t data_bytes)
         {
             const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t *>(data)};
             if (out.channels() == 1)
             {
                 cv::cvtColor(in, out, cv::COLOR_RGB2GRAY);
             }
             else
             {
                 cv::cvtColor(in, out, cv::COLOR_RGB2BGRA);
             }
         }},
        {V4L2_PIX_FMT_YUYV, [](const uint8_t *data, cv::Mat &out, const cv::Size dims, const size_t data_bytes)
         {
             const cv::Mat in{dims, CV_8UC2, const_cast<uint8_t *>(data)};

             if (out.channels() == 1)
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2GRAY_YUYV);
             }
             else
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_YUYV);
             }
         }},
        {V4L2_PIX_FMT_YVYU, [](const uint8_t *data, cv::Mat &out, const cv::Size dims, const size_t data_bytes)
         {
             const cv::Mat in{dims, CV_8UC2, const_cast<uint8_t *>(data)};

             if (out.channels() == 1)
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2GRAY_YVYU);
             }
             else
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_YVYU);
             }
         }},
        {V4L2_PIX_FMT_UYVY, [](const uint8_t *data, cv::Mat &out, const cv::Size dims, const size_t data_bytes)
         {
             const cv::Mat in{dims, CV_8UC2, const_cast<uint8_t *>(data)};

             if (out.channels() == 1)
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2GRAY_UYVY);
             }
             else
             {
                 cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_UYVY);
             }
         }}};
}

bool decoder::decoder_available(__u32 pixel_format)
{
    // check if it is mjpeg or a simple format
    return decoder_mjpeg::v4l_format() == pixel_format ||
           supported_simple_formats.find(pixel_format) != supported_simple_formats.end();
}

std::unique_ptr<decoder> decoder::create(__u32 pixel_format, size_t width, size_t height)
{

    // check for specialized decoders
    if (pixel_format == decoder_mjpeg::v4l_format())
    {
        return std::make_unique<decoder_mjpeg>(width, height);
    }

    // check for a simple decoder
    auto d = supported_simple_formats.find(pixel_format);
    if (d != supported_simple_formats.end())
    {
        return std::make_unique<decoder_simple>(pixel_format, width, height);
    }

    throw std::invalid_argument("Unable to create a decoder for the specified pixel format");
}

void decoder_simple::decode(const uint8_t *data, cv::Mat &output, size_t data_bytes)
{
    // cast away this const so it can be used to create a cv::Mat
    m_decoder_fn(data, output, m_mat_size, data_bytes);
}

decoder_simple::decoder_simple(__u32 pixel_format, size_t width, size_t height)
    : decoder(), m_decoder_fn(supported_simple_formats[pixel_format]), m_mat_size(cv::Size{static_cast<int>(width), static_cast<int>(height)})
{
}