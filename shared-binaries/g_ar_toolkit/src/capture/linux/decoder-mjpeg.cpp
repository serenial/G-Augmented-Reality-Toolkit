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

#include <stdexcept>

#include <linux/videodev2.h>

#include <g_ar_toolkit/capture/linux/decoder-mjpeg.hpp>

using namespace g_ar_toolkit;
using namespace capture;

__u32 decoder_mjpeg::v4l_format()
{
    return V4L2_PIX_FMT_MJPEG;
}

decoder_mjpeg::decoder_mjpeg(int width, int height)
    : m_avcodec(avcodec_find_decoder(AVCodecID::AV_CODEC_ID_MJPEG)),
      m_avparser(av_parser_init(AVCodecID::AV_CODEC_ID_MJPEG)),
      m_avframe_device(av_frame_alloc()),
      m_avoptions(nullptr),
      m_sws_context(nullptr),
      m_averror_str(reinterpret_cast<char *>(malloc(AV_ERROR_MAX_STRING_SIZE))),
      m_width(width), m_height(height)
{
    if (!m_avcodec)
    {
        throw std::runtime_error("Could not find MJPEG decoder.");
    }

    if (!m_avparser)
    {
        throw std::runtime_error("Could not find MJPEG parser.");
    }

    // allocate codec context
    m_avcodec_context = avcodec_alloc_context3(m_avcodec);
    if (!m_avcodec_context)
    {
        throw std::runtime_error("Could not allocate MJPEG codec context.");
    }

    // suppress warnings from ffmpeg libraries to avoid spamming the console
    av_log_set_level(AV_LOG_PANIC);
    av_log_set_flags(AV_LOG_SKIP_REPEATED);

    // set basic codec parameters - let the decoder determine the pixel format
    m_avcodec_context->width = m_width;
    m_avcodec_context->height = m_height;
    m_avcodec_context->codec_type = AVMEDIA_TYPE_VIDEO;

    // initailize AVCodecContext
    if (avcodec_open2(m_avcodec_context, m_avcodec, &m_avoptions) < 0)
    {
        throw std::runtime_error("Could not open MJPEG decoder.");
    }

    // allocate packet
    m_packet = av_packet_alloc();
    if (!m_packet)
    {
        throw std::runtime_error("Could not allocate MJPEG packet.");
    }

    // sws_context and frame buffer will be created on first decode when we know the actual pixel format
}

decoder_mjpeg::~decoder_mjpeg()
{
    if (m_averror_str)
    {
        free(m_averror_str);
    }
    if (m_avoptions)
    {
        free(m_avoptions);
    }
    if (m_avcodec_context)
    {
        avcodec_free_context(&m_avcodec_context);
    }
    if (m_avframe_device)
    {
        av_frame_free(&m_avframe_device);
    }
    if (m_avparser)
    {
        av_parser_close(m_avparser);
    }
    if (m_sws_context)
    {
        sws_freeContext(m_sws_context);
    }
    if (m_packet)
    {
        av_packet_free(&m_packet);
    }
}

void decoder_mjpeg::decode(const uint8_t *data, cv::Mat &dst, size_t size)
{
    if (!data || size == 0)
    {
        throw std::invalid_argument("Decoding failed. Data pointer is invalid or zero size.");
    }

    m_result = 0;

    // Set packet data
    std::memset(m_packet, 0, sizeof(AVPacket));
    m_packet->data = const_cast<uint8_t *>(data);
    m_packet->size = size;

    // Send packet to decoder
    m_result = avcodec_send_packet(m_avcodec_context, m_packet);
    if (m_result < 0)
    {
        throw std::runtime_error("Sending a packet into the codec during decoding failed.");
    }

    // Receive decoded frame
    m_result = avcodec_receive_frame(m_avcodec_context, m_avframe_device);
    if (m_result == AVERROR(EAGAIN) || m_result == AVERROR_EOF)
    {
        return;
    }
    else if (m_result < 0)
    {
        throw std::runtime_error("Receiving a frame from the codec during decoding failed.");
    }

    // Create or recreate sws_context if needed (format changed or first time)
    if (!m_sws_context || 
        m_avframe_device->width != m_width ||
        m_avframe_device->height != m_height)
    {
        if (m_sws_context)
        {
            sws_freeContext(m_sws_context);
        }

        // Use the ACTUAL pixel format from the decoded frame
        m_sws_context = sws_getContext(
            m_avframe_device->width, m_avframe_device->height, 
            (AVPixelFormat)m_avframe_device->format,  // Actual decoded format
            m_width, m_height, cv_mat_type_as_avpixelformat, 
            SWS_FAST_BILINEAR, NULL, NULL, NULL);

        if (!m_sws_context)
        {
            throw std::runtime_error("Could not create SwsContext for pixel format conversion.");
        }
    }

    // Allocate cv::Mat if needed
    if (dst.empty() || dst.cols != m_width ||
        dst.rows != m_height || dst.type() != CV_8UC4)
    {
        dst = cv::Mat(m_height, m_width, CV_8UC4);
    }

    // Setup destination pointers for sws_scale
    uint8_t *dst_data[1] = {dst.data};
    int dst_linesize[1] = {static_cast<int>(dst.step[0])};

    // Convert from decoded format to BGRA (OpenCV format with alpha)
    sws_scale(
        m_sws_context,
        m_avframe_device->data, m_avframe_device->linesize,
        0, m_avframe_device->height,
        dst_data, dst_linesize
    );
}