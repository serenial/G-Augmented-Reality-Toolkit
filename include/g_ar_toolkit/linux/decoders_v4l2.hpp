#ifdef __linux__

#ifndef G_AR_TK__INTEROP_LV_DECODERS_HPP_
#define G_AR_TK__INTEROP_LV_DECODERS_HPP_

#include <exception>
#include <functional>

#include <opencv2/imgcodecs.hpp>

#include "./context_v4l2.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        // abstract class to allow all the decoder types to be put in one bucket
        class decoder{
            public:
            decoder() = default;
            virtual void decode(const uint8_t*, cv::Mat&) = 0;
            static bool decoder_available(__u32);
        };

        // decode RGB data
        class decoder_rgb : public decoder {
            public:
            std::optional<decoder_rgb> create_rgb_decoder(__u32, size_t, size_t);
            decoder_rgb() = delete;
            void decode(const uint8_t*, cv::Mat&) override;
            private:
            decoder_rgb(__u32, size_t, size_t);
            std::function<void(const uint8_t*, cv::Mat&, const cv::Size)> m_decoder_fn;
            cv::Size m_mat_size;
        };

        // decode YUV data
        class decoder_yuv : public decoder {
            public:
            std::optional<decoder_yuv> create_yuv_decoder(__u32, size_t, size_t);
            decoder_yuv() = delete;
            void decode(const uint8_t*, cv::Mat&) override;
            private:
            decoder_yuv(__u32, size_t, size_t);
            std::function<void(const uint8_t*, cv::Mat&, const cv::Size)> m_decoder_fn;
            cv::Size m_mat_size;
        };
    }
}

#endif // G_AR_TK__INTEROP_LV_DECODERS_HPP_
#endif // __liniux__