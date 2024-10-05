#ifdef __linux__

#ifndef G_AR_TK__CAPTURE_LV_DECODERS_HPP_
#define G_AR_TK__CAPTURE_LV_DECODERS_HPP_

#include <exception>
#include <functional>
#include <optional>
#include <memory>

#include <linux/videodev2.h>

#include <opencv2/imgcodecs.hpp>

#include "./context_v4l2.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        // abstract class to allow all the decoder types to be put in one bucket
        class decoder
        {
        public:
            decoder() = default;
            virtual void decode(const uint8_t *, cv::Mat &, size_t) = 0;
            static bool decoder_available(__u32);
            static std::unique_ptr<decoder> create(__u32, size_t, size_t);
        };

        // simple decoders which wrap some opencv channel manipulation logic and don't have any state
        // or allocations which must be explicitly managed
        class decoder_simple : public decoder
        {
        public:
            decoder_simple() = delete;
            void decode(const uint8_t *, cv::Mat &, size_t) override;
            decoder_simple(__u32, size_t, size_t);

        private:
            std::function<void(const uint8_t *, cv::Mat &, const cv::Size, const size_t)> m_decoder_fn;
            cv::Size m_mat_size;
        };
    }
}

#endif // G_AR_TK__CAPTURE_LV_DECODERS_HPP_
#endif // __liniux__