#ifdef __linux__

#ifndef G_AR_TK__CAPTURE_LV_DECODERS_HPP_
#define G_AR_TK__CAPTURE_LV_DECODERS_HPP_

#include <exception>
#include <functional>
#include <optional>

#include <linux/videodev2.h>

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

        // simple decoders which wrap some opencv channel manipulation logic and don't have any state
        // or allocations which must be explicitly managed
        class decoder_simple : public decoder {
            public:
            std::optional<decoder_simple> create_decoder_simple(__u32, size_t, size_t);
            decoder_simple() = delete;
            void decode(const uint8_t*, cv::Mat&) override;
            private:
            decoder_simple(__u32, size_t, size_t);
            std::function<void(const uint8_t*, cv::Mat&, const cv::Size)> m_decoder_fn;
            cv::Size m_mat_size;
        };
    }
}

#endif // G_AR_TK__CAPTURE_LV_DECODERS_HPP_
#endif // __liniux__