#include <stdexcept>
#include <Mferror.h>
#include <future>
#include <thread>
#include <chrono>
#include <optional>

#include <Wmcodecdsp.h>
#include <winrt/base.h>

#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/win/source_reader.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

namespace
{
    std::optional<SourceReader::parameters> stream_param_to_source_reader_param(Stream::camera_parameters param)
    {
        switch (param)
        {
        case Stream::camera_parameters::EXPOSURE:
            return SourceReader::parameters::EXPOSURE;
        case Stream::camera_parameters::FOCUS:
            return SourceReader::parameters::FOCUS;
        case Stream::camera_parameters::ZOOM:
            return SourceReader::parameters::ZOOM;
        case Stream::camera_parameters::WHITE_BALANCE_TEMPERATURE:
            return SourceReader::parameters::WHITE_BALANCE_TEMPERATURE;
        case Stream::camera_parameters::BRIGHTNESS:
            return SourceReader::parameters::BRIGHTNESS;
        case Stream::camera_parameters::CONTRAST:
            return SourceReader::parameters::CONTRAST;
        case Stream::camera_parameters::SATURATION:
            return SourceReader::parameters::SATURATION;
        case Stream::camera_parameters::GAMMA:
            return SourceReader::parameters::GAMMA;
        case Stream::camera_parameters::HUE:
            return SourceReader::parameters::HUE;
        case Stream::camera_parameters::SHARPNESS:
            return SourceReader::parameters::SHARPNESS;
        case Stream::camera_parameters::BACKLIGHT_COMPENSATION:
            return SourceReader::parameters::BACKLIGHT_COMPENSATION;
        case Stream::camera_parameters::POWER_LINE_FREQUENCY:
            return SourceReader::parameters::POWER_LINE_FREQUENCY;
        case Stream::camera_parameters::GAIN:
            return SourceReader::parameters::GAIN;
        }
        return {};
    }

    std::optional<SourceReader::auto_parameters> stream_auto_param_to_source_reader_auto_param(Stream::camera_auto_parameters param)
    {
        switch (param)
        {
        case Stream::camera_auto_parameters::AUTO_EXPOSURE:
            return SourceReader::auto_parameters::AUTO_EXPOSURE;
        case Stream::camera_auto_parameters::AUTO_FOCUS:
            return SourceReader::auto_parameters::AUTO_FOCUS;
        case Stream::camera_auto_parameters::AUTO_WHITE_BALANCE:
            return SourceReader::auto_parameters::AUTO_WHITE_BALANCE;
        case Stream::camera_auto_parameters::AUTO_GAIN:
            return SourceReader::auto_parameters::AUTO_GAIN;
        }
        return {};
    }
}

// Use a thread to manage all Stream Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization

Stream::Stream(std::string_view device_id, stream_type_t stream_type)
    : m_last_state(states::STARTING),
      m_buffer_mat(cv::Mat(stream_type.height, stream_type.width, CV_8UC4)),
      m_rows(stream_type.height),
      m_cols(stream_type.width),
      m_last_exception(nullptr),
      m_ftr(std::async(std::launch::async,
                       [&]()
                       {
                           winrt::com_ptr<SourceReader> camera;
                           {
                               // new lock scope
                               std::lock_guard lk(m_mtx);
                               try
                               {
                                   m_last_exception = nullptr;
                                   camera.attach(SourceReader::create(
                                       device_id,
                                       stream_type.width,
                                       stream_type.height,
                                       1.0f,
                                       [&](winrt::hresult_error ex, IMFSample *pSample)
                                       {
                // sample handler callback function
                if (pSample)
                {
                    // check if someone is waiting for a sample
                    std::unique_lock<std::mutex> lk(m_mtx);
                    if (m_last_state == states::WAITING_ON_CAPTURE)
                    {
                        try
                        {
                            m_last_exception = nullptr;
                            // copy sample to dest_mat_ptr
                            if (m_dest_mat_ptr)
                            {
                                winrt::com_ptr<IMFMediaBuffer> media_buf;
                                winrt::com_ptr<IMF2DBuffer2> media_buf_2d;
                                BYTE *pix;
                                LONG stride;

                                winrt::check_hresult(pSample->GetBufferByIndex(0, media_buf.put()));
                                media_buf_2d = media_buf.as<IMF2DBuffer2>();

                                winrt::check_hresult(media_buf_2d->Lock2D(&pix, &stride));

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

                                winrt::check_hresult(media_buf_2d->Unlock2D());
                            }
                        }
                        catch (...)
                        {
                            m_last_exception = std::current_exception();
                        }
                        m_last_state = states::WAITING_ON_ACTION;
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
                } }));

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

                                   camera->stop_streaming();

                                   lk.unlock();
                                   goto done;

                               case states::WAITING_ON_STREAM_START:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       camera->start_streaming();
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                               case states::WAITING_ON_STREAM_STOP:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       camera->stop_streaming();
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                               case states::WAITING_ON_STREAM_PARAM_GET:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       auto info = camera->get_source_parameter(stream_param_to_source_reader_param(m_camera_parameter_arg).value());
                                       m_last_param_info.default_value = info.default_value;
                                       m_last_param_info.step = info.step;
                                       m_last_param_info.min = info.min;
                                       m_last_param_info.max = info.max;
                                       m_last_param_info.is_supported = info.is_supported;
                                       m_last_param_value = info.value;
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                               case states::WAITING_ON_STREAM_PARAM_AUTO_GET:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       m_last_auto_param_is_automatic = camera->get_source_parameter_auto_mode(stream_auto_param_to_source_reader_auto_param(m_camera_auto_parameter_arg).value());
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                               case states::WAITING_ON_STREAM_PARAM_SET:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       camera->set_source_parameter(stream_param_to_source_reader_param(m_camera_parameter_arg).value(), m_last_param_value);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                                case states::WAITING_ON_STREAM_PARAM_AUTO_SET:
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       camera->set_source_parameter_auto_mode(stream_auto_param_to_source_reader_auto_param(m_camera_auto_parameter_arg).value(), m_last_auto_param_is_automatic);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::WAITING_ON_ACTION;
                                   lk.unlock();
                                   m_notifier.notify_all();
                                   break;

                               default:
                                   // WAITING_ON_CAPTURE handled by Capture-Sample-Handler callback
                                   lk.unlock();
                               }
                           }

                       done:
                           // set state to STOPPED
                           {
                               std::lock_guard lk(m_mtx);
                               m_last_state = states::STOPPED;
                               // release the camera
                               camera->Release();
                           }
                           m_notifier.notify_all();
                       }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::STARTING; });

    if (m_last_state != states::WAITING_ON_ACTION)
    {
        lk.unlock();
        if (m_last_exception)
        {
            std::rethrow_exception(m_last_exception);
        }
    }
    lk.unlock();
}

Stream::~Stream()
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

void Stream::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
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

void Stream::start_stream()
{
    // create new scope, request stream start
    {
        std::lock_guard lk(m_mtx);
        // signal to start streaming
        m_last_state = states::WAITING_ON_STREAM_START;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_START; });
    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(m_last_exception);
    }
    lk.unlock();
}

void Stream::stop_stream()
{
    // create new scope, request stream stop
    {
        std::lock_guard lk(m_mtx);
        // signal to stop streaming
        m_last_state = states::WAITING_ON_STREAM_STOP;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_STOP; });
    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        std::rethrow_exception(m_last_exception);
    }
    lk.unlock();
}

Stream::get_camera_parameter_info(Stream::camera_parameters param, Stream::param_info_t* info)
{
    info->is_supported = true;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::WAITING_ON_STREAM_PARAM_GET;
        m_camera_parameter_arg = param;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_GET; });

    if (m_last_exception)
    {
        info->is_supported = false;
    }
    lk.unlock();
    info->default_value = m_last_param_info.default_value;
    info->max = m_last_param_info.max;
    info->min = m_last_param_info.min;
    info->step = m_last_param_info.step;
}

int32_t Stream::get_camera_parameter(Stream::camera_parameters param)
{
    int32_t value;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::WAITING_ON_STREAM_PARAM_GET;
        m_camera_parameter_arg = param;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_GET; });

    if (m_last_exception)
    {
        lk.unlock();
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }
    value = m_last_param_value;
    lk.unlock();
    return value;
}

void Stream::set_camera_parameter(Stream::camera_parameters param, int32_t value)
{
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::WAITING_ON_STREAM_PARAM_SET;
        m_camera_parameter_arg = param;
        m_last_param_value = value;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_SET; });

    if (m_last_exception)
    {
        lk.unlock();
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }
    lk.unlock();
}

bool Stream::get_camera_auto_mode(Stream::camera_auto_parameters param)
{
    bool is_auto = false;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::WAITING_ON_STREAM_PARAM_AUTO_GET;
        m_camera_auto_parameter_arg = param;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_AUTO_GET; });

    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        throw Stream::auto_param_error{param, "Automatic/Manual control of parameter %s not supported."};
    }
    is_auto = m_last_auto_param_is_automatic;
    lk.unlock();
    return is_auto;
}

void Stream::set_camera_auto_mode(camera_auto_parameters param, bool automatic)
{
        bool is_auto = false;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::WAITING_ON_STREAM_PARAM_AUTO_SET;
        m_camera_auto_parameter_arg = param;
        m_last_auto_param_is_automatic = automatic;
        m_notifier.notify_all();
    }

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_AUTO_SET; });

    if (m_last_exception)
    {
        m_last_state = states::STOPPING;
        lk.unlock();
        throw Stream::auto_param_error{param, "Automatic/Manual control of parameter %s not supported."};
    }
    lk.unlock();
}