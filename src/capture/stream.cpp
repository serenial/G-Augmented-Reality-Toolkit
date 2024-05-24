#include "g_ar_toolkit/lv-interop/lv-image.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;

Stream::Stream(){}
Stream::~Stream(){}
void Stream::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout){}
void Stream::start_stream(){}
void Stream::stop_stream(){}