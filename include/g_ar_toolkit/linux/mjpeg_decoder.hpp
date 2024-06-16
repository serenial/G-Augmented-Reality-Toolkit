#ifdef __linux__

#ifndef G_AR_TK__INTEROP_LV_MJPEG_DECODER_HPP_
#define G_AR_TK__INTEROP_LV_MJPEG_DECODER_HPP_

#include <opencv2/imgcodecs.hpp>

#include "./context_v4l2.hpp"

namespace g_ar_toolkit
{
    namespace capture
    {
        class MJPEGDecoder : public g_ar_toolkit::capture::CompressedDecoder
        {
            public:
            MJPEGDecoder();
            ~MJPEGDecoder();
            void decode(const cv::Mat&, cv::Mat&);
            private:
            cv::Mat m_tmp;
        };
    }
}

#endif // G_AR_TK__INTEROP_LV_MJPEG_DECODER_HPP_
#endif // __liniux__