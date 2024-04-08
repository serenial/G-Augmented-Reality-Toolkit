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

Stream* capture::create_platform_stream(std::string device_id, stream_type_t stream_type)
{
    return new StreamWMF(device_id, stream_type);
}

// Use a thread to manage all Stream Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization

StreamWMF::StreamWMF(std::string device_id, stream_type_t stream_type)
    : Stream(), last_state(states::STARTING),
      buffer_mat(cv::Mat(stream_type.height, stream_type.width, CV_8UC4)),
      rows(stream_type.height),
      cols(stream_type.width),
      last_exception(nullptr),
      streaming(false),
      ftr(std::async(std::launch::async,
                     [&]()
                     {
                         winrt::com_ptr<WindowsMFCapture> camera;
                         {
                             // new lock scope
                             std::lock_guard lk(mtx);
                             try
                             {
                                 last_exception = nullptr;
                                 camera.attach(WindowsMFCapture::CreateInstance(true, winrt::to_hstring(device_id), true));
                                 camera->ChangeCaptureConfig(stream_type.width, stream_type.height, stream_type.fps.numerator, MFVideoFormat_ARGB32, true);
                                 sample_handler_token = camera->AddSampleHandler([&](winrt::hresult_error ex, winrt::hstring msg, IMFSample *pSample)
                                                                                 {
                                     // sample handler callback function                                                               
                                     if (pSample)
                                     {
                                        // check if someone is waiting for a sample
                                        std::unique_lock<std::mutex> lk(mtx);
                                        if (last_state == states::WAITING_ON_CAPTURE)
                                        {
                                            try{
                                            last_exception = nullptr;
                                            on_sample(pSample);
                                            // signal that the buffer is filled and waiting for copy
                                            last_state = states::WAITING_ON_CAPTURE_ACK;
                                            }
                                            catch (...){
                                                last_state = states::WAITING_ON_ACTION;
                                                last_exception = std::current_exception();
                                            }
                                            lk.unlock();
                                            notifier.notify_all();
                                        }    
                                     }
                                     else
                                     {
                                         if ((HRESULT)ex.code().value == MF_E_END_OF_STREAM)
                                         {
                                             std::unique_lock lk(mtx);
                                             last_state = states::WAITING_ON_ACTION;
                                             lk.unlock();
                                             notifier.notify_all();
                                         }
                                     } });

                                 // update state
                                 last_state = states::WAITING_ON_ACTION;
                             }
                             catch (...)
                             {
                                 last_exception = std::current_exception();
                                 // update state
                                 last_state = states::STOPPING;
                             }
                         }
                         // setup done
                         notifier.notify_one();

                         // main thread loop
                         // check for external changes to last_state
                         while (1)
                         {
                             std::unique_lock<std::mutex> lk(mtx);
                             // wait for the state to change
                             notifier.wait(lk, [&]
                                           { return last_state != states::WAITING_ON_ACTION; });

                             switch (last_state)
                             {
                             case states::STOPPING:
                                 if (streaming)
                                 {
                                     camera->StopStreaming();
                                     streaming = false;
                                 }
                                 lk.unlock();
                                 goto done;
                                 break;
                             case states::WAITING_ON_STREAM_START:
                                 try
                                 {      
                                    last_exception = nullptr;
                                     camera->StartStreaming();
                                     // signal that streaming has started
                                     last_state = states::WAITING_ON_STREAM_START_ACK;
                                 }
                                 catch (...)
                                 {
                                     last_state = states::WAITING_ON_ACTION;
                                     last_exception = std::current_exception();
                                 }
                                 lk.unlock();
                                 notifier.notify_all();
                                 break;
                             case states::WAITING_ON_STREAM_STOP:
                                 try
                                 {
                                    last_exception =nullptr;
                                     camera->StopStreaming();
                                     // signal that streaming has started
                                     last_state = states::WAITING_ON_STREAM_STOP_ACK;
                                 }
                                 catch (...)
                                 {
                                     last_state = states::WAITING_ON_ACTION;
                                     last_exception = std::current_exception();
                                 }
                                 lk.unlock();
                                 notifier.notify_all();
                                 break;
                             default:
                                 // WAITING_ON_CAPTURE handled by Capture-Sample-Handler callback
                                 lk.unlock();
                                 break;
                             }
                         }

                     done:
                         // set state to STOPPED
                         {
                             std::lock_guard lk(mtx);
                             last_state = states::STOPPED;
                         }
                         notifier.notify_one();
                     }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(mtx);
    notifier.wait(lk, [&]
                  { return last_state != states::STARTING; });

    if (last_state != states::WAITING_ON_ACTION)
    {
        lk.unlock();
        if (last_exception){
            std::rethrow_exception(last_exception);
        }
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
            notifier.notify_all();
        }
        else
        {
            lk.unlock();
        }
        // wait on future to return
        ftr.wait();
    }
}

void StreamWMF::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
    // create new scope, setup for and request capture
    {
        std::lock_guard lk(mtx);
        if (destination.rows != rows || destination.cols != cols)
        {
            // resize destination
            destination = cv::Mat(rows, cols, CV_8UC4);
        }
        dest_mat_ptr = &destination;
        // signal to start waiting for a capture
        last_state = states::WAITING_ON_CAPTURE;
        // wait timeout for state to change
        notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(mtx);
    if (notifier.wait_for(lk, timeout, [&]
                          { return last_state != states::WAITING_ON_CAPTURE; }))

    {
        if (last_state == states::WAITING_ON_CAPTURE_ACK)
        {
            // update state
            last_state = states::WAITING_ON_ACTION;
        }
    }
    // reset dest_mat_ptr
    dest_mat_ptr = nullptr;
    if (last_exception)
    {
        last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(last_exception);
    }
    lk.unlock();
}

void StreamWMF::start_stream()
{
    // create new scope, request stream start
    {
        std::lock_guard lk(mtx);
        if (streaming)
        {
            return;
        }
        // signal to start streaming
        last_state = states::WAITING_ON_STREAM_START;
        notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(mtx);
    notifier.wait(lk, [&]
                  { return last_state != states::WAITING_ON_STREAM_START; });

    if (last_state == states::WAITING_ON_STREAM_START_ACK)
    {
        // update state
        streaming = true;
        last_state = states::WAITING_ON_ACTION;
    }
    if (last_exception)
    {
        last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(last_exception);
    }
    lk.unlock();
}

void StreamWMF::stop_stream()
{
    // create new scope, request stream stop
    {
        std::lock_guard lk(mtx);
        if (!streaming)
        {
            return;
        }
        // signal to stop streaming
        last_state = states::WAITING_ON_STREAM_STOP;
        notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(mtx);
    notifier.wait(lk, [&]
                  { return last_state != states::WAITING_ON_STREAM_STOP; });

    if (last_state == states::WAITING_ON_STREAM_STOP_ACK)
    {
        streaming = false;
        // update state
        last_state = states::WAITING_ON_ACTION;
    }
    if (last_exception)
    {
        last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(last_exception);
    }
    lk.unlock();
}

void StreamWMF::on_sample(IMFSample *pSample)
{
    if (pSample && dest_mat_ptr)
    {
        winrt::com_ptr<IMFMediaBuffer> spMediaBuf;
        winrt::com_ptr<IMF2DBuffer2> spMediaBuf2d;
        BYTE *pix;
        LONG stride;

        winrt::check_hresult(pSample->GetBufferByIndex(0, spMediaBuf.put()));
        spMediaBuf2d = spMediaBuf.as<IMF2DBuffer2>();

        winrt::check_hresult(spMediaBuf2d->Lock2D(&pix, &stride));

        bool y_flipped = false;
        if (stride < 0)
        {
            stride = -stride;
            y_flipped = true;
        }

        // wrap the buffer into a cv::Mat (no copy)
        cv::Mat mat_from_ptr(rows, cols, CV_8UC4, pix, stride);

        // handle writing mat_from_ptr into dest_mat_ptr - trying to avoid allocations/copies
        cv::Mat *src = &mat_from_ptr;
        if (y_flipped)
        {
            cv::flip(mat_from_ptr, buffer_mat, 0);
            src = &buffer_mat;
        }
        src->copyTo(*dest_mat_ptr);

        winrt::check_hresult(spMediaBuf2d->Unlock2D());
    }
}