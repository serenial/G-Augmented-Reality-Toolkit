#include <stdexcept>
#include <string>
#include <linux/videodev2.h>
#include "g_ar_toolkit/capture/linux/decoder-mjpeg.hpp"

using namespace g_ar_toolkit;
using namespace capture;

__u32 decoder_mjpeg::v4l_format()
{
    return V4L2_PIX_FMT_MJPEG;
}

decoder_mjpeg::decoder_mjpeg(int width, int height)
    : m_width(width), m_height(height), m_decompressor(nullptr)
{
    // Create TurboJPEG decompressor instance
    m_decompressor = tjInitDecompress();
    
    if (!m_decompressor)
    {
        throw std::runtime_error("Failed to initialize the decompressor.");
    }
}

decoder_mjpeg::~decoder_mjpeg()
{
    if (m_decompressor)
    {
        tjDestroy(m_decompressor);
        m_decompressor = nullptr;
    }
}

void decoder_mjpeg::decode(const uint8_t *data, cv::Mat &dst, size_t size)
{
    if (!m_decompressor)
    {
        throw std::runtime_error("Decompressor not initialized.");
    }

    if (!data || size == 0)
    {
        throw std::runtime_error("Invalid data pointer or zero size");
    }

    // Get JPEG image info
    int width, height, jpegSubsamp, jpegColorspace;
    
    if (tjDecompressHeader3(m_decompressor, 
                            const_cast<unsigned char*>(data), 
                            size,
                            &width, 
                            &height, 
                            &jpegSubsamp, 
                            &jpegColorspace) != 0)
    {
        throw std::runtime_error("Failed to read JPEG header: " + std::string(tjGetErrorStr2(m_decompressor)) + ".");
    }

    // Allocate cv::Mat if needed (BGRA format)
    if (dst.empty() || dst.cols != width || dst.rows != height || dst.type() != CV_8UC4)
    {
        dst = cv::Mat(height, width, CV_8UC4);
    }

    // Decompress directly to BGRA format
    if (tjDecompress2(m_decompressor,
                      const_cast<unsigned char*>(data),
                      size,
                      dst.data,
                      width,
                      0,  // pitch (0 = use default)
                      height,
                      TJPF_BGRA,  // TurboJPEG pixel format (BGRA)
                      TJFLAG_FASTDCT) != 0)  // Use fast DCT for speed
    {
        throw std::runtime_error("Failed to decompress JPEG header: " + std::string(tjGetErrorStr2(m_decompressor)) + ".");
    }
}