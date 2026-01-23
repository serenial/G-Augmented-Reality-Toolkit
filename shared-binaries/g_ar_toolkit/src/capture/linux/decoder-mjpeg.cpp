#include <stdexcept>
#include <cstring>
#include <linux/videodev2.h>
#include "g_ar_toolkit/capture/linux/decoder-mjpeg.hpp"

using namespace g_ar_toolkit;
using namespace capture;

__u32 decoder_mjpeg::v4l_format()
{
    return V4L2_PIX_FMT_MJPEG;
}

decoder_mjpeg::decoder_mjpeg(int width, int height)
    : m_width(width), m_height(height), m_buffer(nullptr), m_row_stride(0)
{
    // Initialize the JPEG decompression object with error handling
    m_cinfo.err = jpeg_std_error(&m_jerr);
    jpeg_create_decompress(&m_cinfo);
}

decoder_mjpeg::~decoder_mjpeg()
{
    // Clean up the JPEG decompression object
    jpeg_destroy_decompress(&m_cinfo);
    
    // Free the scanline buffer if allocated
    if (m_buffer)
    {
        delete[] m_buffer[0];
        delete[] m_buffer;
    }
}

void decoder_mjpeg::decode(const uint8_t *data, cv::Mat &dst, size_t size)
{
    if (!data || size == 0)
    {
        throw std::invalid_argument("Decoding failed. Data pointer is invalid or zero size.");
    }

    // Set up the data source (JPEG data in memory)
    jpeg_mem_src(&m_cinfo, const_cast<unsigned char*>(data), size);

    // Read the JPEG header
    if (jpeg_read_header(&m_cinfo, TRUE) != JPEG_HEADER_OK)
    {
        throw std::runtime_error("Failed to read JPEG header.");
    }

    // Set output format to BGRA (for OpenCV compatibility with alpha channel)
    // JCS_EXT_BGRA is a libjpeg-turbo extension for direct BGRA output
    m_cinfo.out_color_space = JCS_EXT_BGRA;

    // Start decompression
    if (!jpeg_start_decompress(&m_cinfo))
    {
        throw std::runtime_error("Failed to start JPEG decompression.");
    }

    // Verify dimensions match expected values
    if (static_cast<int>(m_cinfo.output_width) != m_width || 
        static_cast<int>(m_cinfo.output_height) != m_height)
    {
        // Optional: Allow dynamic resizing or throw error
        // For now, we'll adapt to actual decoded dimensions
    }

    int actual_width = m_cinfo.output_width;
    int actual_height = m_cinfo.output_height;

    // Allocate cv::Mat if needed
    if (dst.empty() || dst.cols != actual_width ||
        dst.rows != actual_height || dst.type() != CV_8UC4)
    {
        dst = cv::Mat(actual_height, actual_width, CV_8UC4);
    }

    // Calculate row stride (bytes per row)
    m_row_stride = m_cinfo.output_width * m_cinfo.output_components;

    // Allocate scanline buffer if not already allocated or if size changed
    if (!m_buffer || m_row_stride != static_cast<int>(dst.step[0]))
    {
        if (m_buffer)
        {
            delete[] m_buffer[0];
            delete[] m_buffer;
        }
        
        // Allocate a one-row-high sample array for reading scanlines
        m_buffer = new JSAMPROW[1];
        m_buffer[0] = new JSAMPLE[m_row_stride];
    }

    // Read scanlines directly into the cv::Mat data
    int row = 0;
    while (m_cinfo.output_scanline < m_cinfo.output_height)
    {
        // Point directly to the destination row in cv::Mat
        JSAMPROW row_pointer = dst.ptr<JSAMPLE>(row);
        
        // Read one scanline
        if (jpeg_read_scanlines(&m_cinfo, &row_pointer, 1) != 1)
        {
            jpeg_finish_decompress(&m_cinfo);
            throw std::runtime_error("Failed to read JPEG scanline.");
        }
        
        row++;
    }

    // Finish decompression
    if (!jpeg_finish_decompress(&m_cinfo))
    {
        throw std::runtime_error("Failed to finish JPEG decompression.");
    }
}