#include <stdexcept>
#include <Mferror.h>
#include <future>
#include <thread>
#include <chrono>

#include <Wmcodecdsp.h>
#include <winrt/base.h>

#include "win_camera/wincapture.h"

#include "g_ar_toolkit/win/stream_wmf.hpp"
#include "g_ar_toolkit/win/context_wmf.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;
using namespace ros_win_camera;

Stream* capture::create_platform_stream(std::string_view device_id, stream_type_t stream_type)
{
    return new StreamWMF(device_id, stream_type);
}

// Use a thread to manage all Stream Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization

StreamWMF::StreamWMF(std::string_view device_id, stream_type_t stream_type)
    : Stream(), m_last_state(states::STARTING),
      m_buffer_mat(cv::Mat(stream_type.height, stream_type.width, CV_8UC4)),
      m_rows(stream_type.height),
      m_cols(stream_type.width),
      m_last_exception(nullptr),
      m_streaming(false),
      m_ftr(std::async(std::launch::async,
                     [&]()
                     {
                         winrt::com_ptr<WindowsMFCapture> camera;
                         {
                             // new lock scope
                             std::lock_guard lk(m_mtx);
                             try
                             {
                                 m_last_exception = nullptr;
                                 camera.attach(WindowsMFCapture::CreateInstance(true, winrt::to_hstring(device_id), true));
                                 camera->ChangeCaptureConfig(stream_type.width, stream_type.height, stream_type.fps_numerator, MFVideoFormat_ARGB32, true);
                                 m_sample_handler_token = camera->AddSampleHandler([&](winrt::hresult_error ex, winrt::hstring msg, IMFSample *pSample)
                                                                                 {
                                     // sample handler callback function                                                               
                                     if (pSample)
                                     {
                                        // check if someone is waiting for a sample
                                        std::unique_lock<std::mutex> lk(m_mtx);
                                        if (m_last_state == states::WAITING_ON_CAPTURE)
                                        {
                                            try{
                                            m_last_exception = nullptr;
                                            on_sample(pSample);
                                            // signal that the buffer is filled and waiting for copy
                                            m_last_state = states::WAITING_ON_CAPTURE_ACK;
                                            }
                                            catch (...){
                                                m_last_state = states::WAITING_ON_ACTION;
                                                m_last_exception = std::current_exception();
                                            }
                                            lk.unlock();
                                            m_notifier.notify_all();
                                        }    
                                     }
                                     else
                                     {
                                         if ((HRESULT)ex.code().value == MF_E_END_OF_STREAM)
                                         {
                                             std::unique_lock lk(m_mtx);
                                             m_last_state = states::WAITING_ON_ACTION;
                                             lk.unlock();
                                             m_notifier.notify_all();
                                         }
                                     } });

                                 // update state
                                 m_last_state = states::WAITING_ON_ACTION;
                             }
                             catch (...)
                             {
                                 m_last_exception = std::current_exception();
                                 // update state
                                 m_last_state = states::STOPPING;
                             }
                         }
                         // setup done
                         m_notifier.notify_one();

                         // main thread loop
                         // check for external changes to last_state
                         while (1)
                         {
                             std::unique_lock<std::mutex> lk(m_mtx);
                             // wait for the state to change
                             m_notifier.wait(lk, [&]
                                           { return m_last_state != states::WAITING_ON_ACTION; });

                             switch (m_last_state)
                             {
                             case states::STOPPING:
                                 if (m_streaming)
                                 {
                                     camera->StopStreaming();
                                     m_streaming = false;
                                 }
                                 lk.unlock();
                                 goto done;
                                 break;
                             case states::WAITING_ON_STREAM_START:
                                 try
                                 {      
                                    m_last_exception = nullptr;
                                     camera->StartStreaming();
                                     // signal that streaming has started
                                     m_last_state = states::WAITING_ON_STREAM_START_ACK;
                                 }
                                 catch (...)
                                 {
                                     m_last_state = states::WAITING_ON_ACTION;
                                     m_last_exception = std::current_exception();
                                 }
                                 lk.unlock();
                                 m_notifier.notify_all();
                                 break;
                             case states::WAITING_ON_STREAM_STOP:
                                 try
                                 {
                                    m_last_exception =nullptr;
                                     camera->StopStreaming();
                                     // signal that streaming has started
                                     m_last_state = states::WAITING_ON_STREAM_STOP_ACK;
                                 }
                                 catch (...)
                                 {
                                     m_last_state = states::WAITING_ON_ACTION;
                                     m_last_exception = std::current_exception();
                                 }
                                 lk.unlock();
                                 m_notifier.notify_all();
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
                             std::lock_guard lk(m_mtx);
                             m_last_state = states::STOPPED;
                         }
                         m_notifier.notify_one();
                     }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(m_mtx);
    m_notifier.wait(lk, [&]
                  { return m_last_state != states::STARTING; });

    if (m_last_state != states::WAITING_ON_ACTION)
    {
        lk.unlock();
        if (m_last_exception){
            std::rethrow_exception(m_last_exception);
        }
    }
    lk.unlock();
}

StreamWMF::~StreamWMF()
{
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        if (m_last_state != states::STOPPED)
        {
            // instruct thread to stop
            m_last_state = states::STOPPING;
            lk.unlock();
            m_notifier.notify_all();
        }
        else
        {
            lk.unlock();
        }
        // wait on future to return
        m_ftr.wait();
    }
}

void StreamWMF::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{
    // create new scope, setup for and request capture
    {
        std::lock_guard lk(m_mtx);
        if (destination.rows != m_rows || destination.cols != m_cols)
        {
            // resize destination
            destination = cv::Mat(m_rows, m_cols, CV_8UC4);
        }
        m_dest_mat_ptr = &destination;
        // signal to start waiting for a capture
        m_last_state = states::WAITING_ON_CAPTURE;
        // wait timeout for state to change
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    if (m_notifier.wait_for(lk, timeout, [&]
                          { return m_last_state != states::WAITING_ON_CAPTURE; }))

    {
        if (m_last_state == states::WAITING_ON_CAPTURE_ACK)
        {
            // update state
            m_last_state = states::WAITING_ON_ACTION;
        }
    }
    // reset dest_mat_ptr
    m_dest_mat_ptr = nullptr;
    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(m_last_exception);
    }
    lk.unlock();
}

void StreamWMF::start_stream()
{
    // create new scope, request stream start
    {
        std::lock_guard lk(m_mtx);
        if (m_streaming)
        {
            return;
        }
        // signal to start streaming
        m_last_state = states::WAITING_ON_STREAM_START;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                  { return m_last_state != states::WAITING_ON_STREAM_START; });

    if (m_last_state == states::WAITING_ON_STREAM_START_ACK)
    {
        // update state
        m_streaming = true;
        m_last_state = states::WAITING_ON_ACTION;
    }
    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(m_last_exception);
    }
    lk.unlock();
}

void StreamWMF::stop_stream()
{
    // create new scope, request stream stop
    {
        std::lock_guard lk(m_mtx);
        if (!m_streaming)
        {
            return;
        }
        // signal to stop streaming
        m_last_state = states::WAITING_ON_STREAM_STOP;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                  { return m_last_state != states::WAITING_ON_STREAM_STOP; });

    if (m_last_state == states::WAITING_ON_STREAM_STOP_ACK)
    {
        m_streaming = false;
        // update state
        m_last_state = states::WAITING_ON_ACTION;
    }
    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(m_last_exception);
    }
    lk.unlock();
}

void StreamWMF::on_sample(IMFSample *pSample)
{
    if (pSample && m_dest_mat_ptr)
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
        cv::Mat mat_from_ptr(m_rows, m_cols, CV_8UC4, pix, stride);

        // handle writing mat_from_ptr into dest_mat_ptr - trying to avoid allocations/copies
        cv::Mat *src = &mat_from_ptr;
        if (y_flipped)
        {
            cv::flip(mat_from_ptr, m_buffer_mat, 0);
            src = &m_buffer_mat;
        }
        src->copyTo(*m_dest_mat_ptr);

        winrt::check_hresult(spMediaBuf2d->Unlock2D());
    }
}