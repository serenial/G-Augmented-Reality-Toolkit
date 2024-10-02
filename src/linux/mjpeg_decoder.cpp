#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/linux/mjpeg_decoder.hpp"

using namespace g_ar_toolkit;
using namespace capture;

MJPEGDecoder::MJPEGDecoder() : CompressedDecoder()
{
}

MJPEGDecoder::~MJPEGDecoder()
{
}

void MJPEGDecoder::decode(const cv::Mat &in, cv::Mat &out)
{
    if (m_tmp.data == nullptr)
        m_tmp = cv::imdecode(const_cast<cv::Mat &>(in), cv::IMREAD_UNCHANGED);
    else
    {
        cv::imdecode(const_cast<cv::Mat &>(in), cv::IMREAD_UNCHANGED, &m_tmp);
    }

    if (m_tmp.data == nullptr)
    {
        throw std::runtime_error("Unable to decode MJPEG image frame");
    }

    switch (m_tmp.cols)
    {
    case 3:
        cv::cvtColor(m_tmp, out, cv::COLOR_BGR2BGRA);
        break;
    case 4:
        m_tmp.copyTo(out);
        break;
    default:
        throw std::runtime_error("Unexpected MJPEG data number of channels");
    }
}
