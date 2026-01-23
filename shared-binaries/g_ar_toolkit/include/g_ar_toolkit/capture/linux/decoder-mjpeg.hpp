// A good portion of this code is based on the work or Evan Flynn
// https://github.com/ros-drivers/usb_cam/blob/main/include/usb_cam/formats/mjpeg.hpp

// Copyright 2023 Evan Flynn
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Evan Flynn nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#pragma once

extern "C"
{
#define __STDC_CONSTANT_MACROS // Required for libavutil
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavutil/error.h"
#include "libavutil/log.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

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
            const AVCodec *m_avcodec;
            AVCodecContext *m_avcodec_context;
            AVCodecParserContext *m_avparser;
            AVFrame *m_avframe_device;
            AVDictionary *m_avoptions;
            SwsContext *m_sws_context;
            AVPacket *m_packet;
            size_t m_avframe_device_size;
            char *m_averror_str;
            int m_result = 0;

            const int m_width, m_height;

            const int m_align = 32;

            const AVPixelFormat cv_mat_type_as_avpixelformat = AVPixelFormat::AV_PIX_FMT_BGRA;
        };
    }
}