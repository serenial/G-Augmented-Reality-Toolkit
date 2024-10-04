#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/linux/decoders_v4l2.hpp"

using namespace g_ar_toolkit;
using namespace capture;

namespace
{
    std::unordered_map<__u32, std::function<void(const uint8_t *, cv::Mat &, const cv::Size)>> supported_rgb_formats = {
        {V4L2_PIX_FMT_BGR24, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t*>(data)};
             cv::cvtColor(in, out, cv::COLOR_BGR2BGRA);
         }},
        {V4L2_PIX_FMT_RGB24, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t*>(data)};
             cv::cvtColor(in, out, cv::COLOR_RGB2BGRA);
         }},
        {V4L2_PIX_FMT_ABGR32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 0, 2, 1, 3, 2}), 4);
         }},
        {V4L2_PIX_FMT_XBGR32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             cv::mixChannels(&in, 1, &out, 1, std::begin({1, 0, 2, 1, 3, 2}), 3);
         }},
        {V4L2_PIX_FMT_BGRA32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             in.copyTo(out);
         }},
        {V4L2_PIX_FMT_BGRX32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         { 
            const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
            cv::mixChannels(&in, 1, &out, 1, std::begin({0, 0, 1, 1, 2, 2}), 3); }},
        {V4L2_PIX_FMT_RGBA32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             cv::mixChannels(&in, 1, &out, 1, std::begin({0, 2, 1, 1, 2, 0, 3, 3}), 4);
         }},
        {V4L2_PIX_FMT_RGBX32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 2, 2, 1}), 3);
         }},
        {V4L2_PIX_FMT_ARGB32, [](const uint8_t *data, cv::Mat &out, const cv::Size dims)
         {
             const cv::Mat in{dims, CV_8UC4, const_cast<uint8_t*>(data)};
             cv::mixChannels(&in, 1, &out, 1, std::begin({0, 3, 1, 2, 2, 1, 3, 0}), 4);
         }}};

    //*****************************************************************//
    //*****************************************************************//
    //*****           F I X  T H E S E  F O R M A T S         *********//
    //*****************************************************************//
    //*****************************************************************//

   std::unordered_map<__u32, std::function<void(const uint8_t *, cv::Mat &, const cv::Size)>> supported_yuv_formats = {
        {V4L2_PIX_FMT_YUYV, [](const uint8_t* data, cv::Mat &out, cv::Size dims)
                             {  const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t*>(data)};
                                cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_YUYV); }},
        {V4L2_PIX_FMT_YVYU, [](const uint8_t* data, cv::Mat &out, cv::Size dims)
                             {  const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t*>(data)};
                                cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_YVYU); }},
        {V4L2_PIX_FMT_UYVY, [](const uint8_t* data, cv::Mat &out, cv::Size dims)
                             {  const cv::Mat in{dims, CV_8UC3, const_cast<uint8_t*>(data)};
                                cv::cvtColor(in, out, cv::COLOR_YUV2BGRA_UYVY); }}};

    //****************************************************************//
    //****************************************************************//
}

bool decoder::decoder_available(__u32 pixel_format){
    return supported_rgb_formats.find(pixel_format)!= supported_rgb_formats.end() 
    || supported_yuv_formats.find(pixel_format)!= supported_yuv_formats.end();
}

std::optional<decoder_rgb> decoder_rgb::create_rgb_decoder(__u32 pixel_format, size_t width, size_t height) {
    // check that we can actually construct this 
    auto match = supported_rgb_formats.find(pixel_format);
    if(match == supported_rgb_formats.end()){
        return std::nullopt;
    }
    return decoder_rgb(pixel_format, width, height);
}

void decoder_rgb::decode(const uint8_t *data, cv::Mat &output)
{
    // cast away this const so it can be used to create a cv::Mat
    m_decoder_fn(data, output, m_mat_size);
}

decoder_rgb::decoder_rgb(__u32 pixel_format, size_t width, size_t height) 
    : decoder(), m_decoder_fn(supported_rgb_formats[pixel_format]), m_mat_size(cv::Size{static_cast<int>(width), static_cast<int>(height)})
{
}

std::optional<decoder_yuv> decoder_yuv::create_yuv_decoder(__u32 pixel_format, size_t width, size_t height) {
    // check that we can actually construct this 
    auto match = supported_yuv_formats.find(pixel_format);
    if(match == supported_yuv_formats.end()){
        return std::nullopt;
    }
    return decoder_yuv(pixel_format, width, height);
}

void decoder_yuv::decode(const uint8_t *data, cv::Mat &output)
{
    // cast away this const so it can be used to create a cv::Mat
    m_decoder_fn(data, output, m_mat_size);
}

decoder_yuv::decoder_yuv(__u32 pixel_format, size_t width, size_t height) 
    : decoder(), m_decoder_fn(supported_yuv_formats[pixel_format]), m_mat_size(cv::Size{static_cast<int>(width), static_cast<int>(height)})
{
}