#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

#include "./decoders.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class decoder_mjpeg : public decoder{
            public:
            static __u32 v4l_format();
            void decode(const uint8_t *, cv::Mat &, size_t) override;
            decoder_mjpeg() = delete;
            decoder_mjpeg(int width, int height);
            ~decoder_mjpeg();
            
            private:
            const AVCodec *m_codec;
            AVCodecContext *m_codec_ctx;
            AVFrame *m_frame;
            AVPacket *m_packet;
            SwsContext *m_sws_ctx;
            
            int m_width;
            int m_height;
        };
    }
}