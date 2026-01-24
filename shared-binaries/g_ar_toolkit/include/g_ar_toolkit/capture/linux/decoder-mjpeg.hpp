#pragma once

#include <turbojpeg.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>

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
            tjhandle m_decompressor;
            const int m_width, m_height;
        };
    }
}