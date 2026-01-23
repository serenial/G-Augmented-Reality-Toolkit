#pragma once

#include <jpeglib.h>
#include <linux/videodev2.h>
#include "./decoders.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class decoder_mjpeg : public decoder
        {
        public:
            static __u32 v4l_format();
            void decode(const uint8_t *, cv::Mat &, size_t) override;
            decoder_mjpeg() = delete;
            decoder_mjpeg(int width, int height);
            ~decoder_mjpeg();

        private:
            struct jpeg_decompress_struct m_cinfo;
            struct jpeg_error_mgr m_jerr;
            
            const int m_width, m_height;
            
            // Temporary buffer for row-by-row decoding
            JSAMPARRAY m_buffer;
            int m_row_stride;
        };
    }
}