#include <stdexcept>

#include <linux/videodev2.h>

#include <g_ar_toolkit/capture/linux/decoder-mjpeg.hpp>

using namespace g_ar_toolkit;
using namespace capture;

__u32 decoder_mjpeg::v4l_format(){
    return V4L2_PIX_FMT_MJPEG;
}
decoder_mjpeg::decoder_mjpeg(int width, int height) 
    : m_codec(avcodec_find_decoder(AV_CODEC_ID_MJPEG)), m_codec_ctx(nullptr), m_frame(nullptr), 
      m_packet(nullptr), m_sws_ctx(nullptr), m_width(width), m_height(height) {
    
    // Check codec was found OK
    if (!m_codec) {
        throw std::runtime_error("MJPEG codec not found");
    }
    
    // Allocate codec context
    m_codec_ctx = avcodec_alloc_context3(m_codec);
    if (!m_codec_ctx) {
        throw std::runtime_error("Could not allocate codec context");
    }
    
    // Set known dimensions
    m_codec_ctx->width = m_width;
    m_codec_ctx->height = m_height;
    
    // Open codec
    if (avcodec_open2(m_codec_ctx, m_codec, nullptr) < 0) {
        avcodec_free_context(&m_codec_ctx);
        throw std::runtime_error("Could not open codec");
    }
    
    // Allocate frame
    m_frame = av_frame_alloc();
    if (!m_frame) {
        avcodec_free_context(&m_codec_ctx);
        throw std::runtime_error("Could not allocate frame");
    }
    
    // Allocate packet
    m_packet = av_packet_alloc();
    if (!m_packet) {
        av_frame_free(&m_frame);
        avcodec_free_context(&m_codec_ctx);
        throw std::runtime_error("Could not allocate packet");
    }
    
    // Pre-allocate scaler context with known dimensions
    m_sws_ctx = sws_getContext(
        m_width, m_height, AV_PIX_FMT_YUVJ422P,  // MJPEG typically uses YUVJ422P
        m_width, m_height, AV_PIX_FMT_BGRA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (!m_sws_ctx) {
        av_packet_free(&m_packet);
        av_frame_free(&m_frame);
        avcodec_free_context(&m_codec_ctx);
        throw std::runtime_error("Could not create scaler context");
    }
}

decoder_mjpeg::~decoder_mjpeg() {
    if (m_sws_ctx) {
        sws_freeContext(m_sws_ctx);
    }
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_codec_ctx) {
        avcodec_free_context(&m_codec_ctx);
    }
}

void decoder_mjpeg::decode(const uint8_t *data, cv::Mat &dst, size_t size) {
    if (!data || size == 0) {
        throw std::invalid_argument("Decoding failed. Data pointer is invalid or zero size.");
    }
    
    // Set packet data
    m_packet->data = const_cast<uint8_t*>(data);
    m_packet->size = size;
    
    // Send packet to decoder
    int ret = avcodec_send_packet(m_codec_ctx, m_packet);
    if (ret < 0) {
        throw std::runtime_error("Sending a packet into the codec during decoding failed.");
    }
    
    // Receive decoded frame
    ret = avcodec_receive_frame(m_codec_ctx, m_frame);
    if (ret < 0) {
        throw std::runtime_error("Recieving a frame from the codec during decoding failed.");
    }
    
    // Allocate cv::Mat if needed
    if (dst.empty() || dst.cols != m_width || 
        dst.rows != m_height || dst.type() != CV_8UC4) {
        dst = cv::Mat(m_height, m_width, CV_8UC4);
    }
    
    // Setup destination pointers for sws_scale
    uint8_t *dst_data[1] = { dst.data };
    int dst_linesize[1] = { static_cast<int>(dst.step[0]) };
    
    // Convert from decoded format to BGRA (OpenCV format with alpha)
    sws_scale(
        m_sws_ctx,
        m_frame->data, m_frame->linesize,
        0, m_frame->height,
        dst_data, dst_linesize
    );
}