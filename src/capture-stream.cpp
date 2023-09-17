#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;

Stream::Stream(){}
Stream::~Stream() {
    auto x = 100;
}
bool Stream::grab_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
    return false;
}

bool Stream::start(){
    return false;
}

bool Stream::stop(){
    return false;
}