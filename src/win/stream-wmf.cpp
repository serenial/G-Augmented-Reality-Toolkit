// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/multimedia/mediafoundation/MFCaptureToFile/capture.cpp

#include <stdexcept>
#include <Mferror.h>
#include <future>
#include <thread>
#include <chrono>

#include <Wmcodecdsp.h>
#include <winrt/base.h>

#include "wincapture.h"

#include "g_ar_toolkit/win/stream-wmf.hpp"
#include "g_ar_toolkit/win/context-wmf.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;
using namespace ros_msft_camera;

Stream *capture::create_platform_stream(std::string device_id, stream_type_t stream_type)
{
    return new StreamWMF(device_id, stream_type);
}

StreamWMF::StreamWMF(std::string device_id, stream_type_t stream_type)
    : Stream(), last_state(states::STARTING),
      last_error(errors::NO_ERR),
      ftr(std::async(std::launch::async,
                     [&]()
                     {
                         winrt::com_ptr<WindowsMFCapture> camera;
                         {
                             // new lock scope
                             std::lock_guard lk(mtx);
                             try
                             {
                                 camera.attach(WindowsMFCapture::CreateInstance(true, winrt::to_hstring(device_id), true));
                                 camera->ChangeCaptureConfig(stream_type.width, stream_type.height, stream_type.fps.numerator, MFVideoFormat_ARGB32, true);
                             }
                             catch (winrt::hresult_error &e)
                             {
                                 last_error = errors::CAPTURE_STREAM_APPLYING_CONFIG;
                             }
                             // update state
                             last_state = last_error == errors::NO_ERR ? states::WAITING_ON_ACTION : states::STOPPING;
                         }
                         // setup done
                         cv.notify_one();

                         // set state to STOPPED
                         {
                             std::lock_guard lk(mtx);
                             last_state = states::STOPPED;
                         }
                         cv.notify_one();
                     }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(mtx);
    cv.wait(lk, [&]
            { return last_state != states::STARTING; });

    if (last_state != states::WAITING_ON_ACTION)
    {
        const char *error_msg = "An unknown error occured whilst initializing the Stream.";

        switch (last_error)
        {
        case errors::COM_INIT_ERR:
            error_msg = "Unable to initiliaze COM components";
            break;
        case errors::MF_STARTUP_BAD_VER:
            error_msg = "The Windows Media Fondation version installed is not compatible with this library.";
            break;
        case errors::MF_STARTUP_OTHER_ERR:
            error_msg = "Unable to initialize the Windows Media Foundation components.";
            break;
        }
        lk.unlock();
        throw std::runtime_error(error_msg);
    }
    lk.unlock();
}

StreamWMF::~StreamWMF()
{
    {
        std::unique_lock<std::mutex> lk(mtx);
        if (last_state != states::STOPPED)
        {
            // instruct thread to stop
            last_state = states::STOPPING;
            lk.unlock();
            cv.notify_one();
        }
        else
        {
            lk.unlock();
        }
        // wait on future to return
        ftr.wait();
    }
}

bool StreamWMF::grab_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
    return false;
}
bool StreamWMF::start()
{
    return false;
}
bool StreamWMF::stop()
{
    return false;
}
