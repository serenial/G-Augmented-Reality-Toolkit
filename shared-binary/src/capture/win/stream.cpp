#include <stdexcept>
#include <Mferror.h>
#include <future>
#include <thread>
#include <chrono>
#include <optional>

#include <Wmcodecdsp.h>
#include <winrt/base.h>

#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/enumerator.hpp"
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

    std::exception_ptr convert_hresult_exception(winrt::hresult_error const &ex)
    {
        return std::make_exception_ptr(std::system_error(std::error_code(ex.code(), std::system_category()), winrt::to_string(ex.message())));
    }

}

// Use a thread to manage all Stream Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization

Stream::Stream(std::string_view device_id, stream_type_t stream_type)
    : m_last_state(states::WAITING_ON_INITIALIZED),
      m_rows(stream_type.height),
      m_cols(stream_type.width),
      m_current_mat(cv::Mat(m_rows, m_cols, CV_8UC4)),
      m_last_exception(nullptr),
      m_image_exception(nullptr),
      m_ftr(std::async(std::launch::async,
                       [&]()
                       {
                           winrt::com_ptr<SourceReader> camera;
                           bool initialized_ok = true;
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
                                       stream_type.fps_numerator,
                                       stream_type.fps_denominator,
                                       [&](SourceReader::callback_events event, IMFSample *pSample)
                                       {
                                           switch (event)
                                           {
                                           case SourceReader::callback_events::END_OF_STREAM:
                                           {
                                               // stream finished
                                               return;
                                           }
                                           case SourceReader::callback_events::EXCEPTION:
                                           {
                                               // similar to END_OF_STREAM but throw exception
                                               {
                                                   std::lock_guard lk(m_mtx);
                                                   m_image_exception = std::make_exception_ptr(std::runtime_error("The camera has reported and error and has stopped streaming."));
                                               }
                                               m_image_notifier.notify_one();
                                           }
                                           case SourceReader::callback_events::NOT_ACCEPTING:
                                           {
                                               // similar to END_OF_STREAM but throw exception
                                               {
                                                   std::lock_guard lk(m_mtx);
                                                   m_image_exception = std::make_exception_ptr(std::runtime_error("The camera has reported it is not excepting more requests at this time."));
                                               };
                                               m_image_notifier.notify_one();
                                           }
                                           case SourceReader::callback_events::SAMPLE:
                                           {
                                               if (pSample)
                                               {

                                                   try
                                                   {
                                                       m_image_exception = nullptr;
                                                       // copy sample to dest_mat_ptr

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

                                                       {
                                                           std::scoped_lock<std::mutex> lk(m_image_mtx);

                                                           // handle writing mat_from_ptr into dest_mat_ptr - trying to avoid allocations/copies
                                                           if (y_flipped)
                                                           {
                                                               cv::flip(mat_from_ptr, m_current_mat, 0);
                                                           }
                                                           else
                                                           {
                                                               mat_from_ptr.copyTo(m_current_mat);
                                                           }

                                                           winrt::check_hresult(media_buf_2d->Unlock2D());
                                                       }
                                                   }
                                                   catch (winrt::hresult_error const &ex)
                                                   {
                                                       m_image_exception = convert_hresult_exception(ex);
                                                   }
                                                   catch (...)
                                                   {
                                                       m_image_exception = std::current_exception();
                                                   }
                                                   m_image_notifier.notify_one();
                                               }
                                               break;
                                           }
                                           }
                                       }));

                                   // update state
                                   m_last_state = states::NOTHING_PENDING;
                               }
                               catch (winrt::hresult_error const &ex)
                               {
                                   m_last_exception = convert_hresult_exception(ex);
                                   m_last_state = states::INTERNAL_THREAD_TERMINATED;
                                   initialized_ok = false;
                               }
                               catch (...)
                               {
                                   m_last_exception = std::current_exception();
                                   m_last_state = states::INTERNAL_THREAD_TERMINATED;
                                   initialized_ok = false;
                               }
                           } // setup done - exit initialization section scoped_lock

                           m_notifier.notify_one();

                           // main thread loop
                           // check for external changes to last_state
                           while (initialized_ok)
                           {
                               std::unique_lock<std::mutex> lk(m_mtx);
                               // wait for the state to change
                               m_notifier.wait(lk, [&]
                                               { return m_last_state != states::NOTHING_PENDING; });

                               switch (m_last_state)
                               {
                               case states::WAITING_ON_DEINITIALIZED:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (camera)
                                       {
                                           camera->stop_streaming();
                                           m_is_streaming = camera->is_streaming();
                                       }
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   try
                                   {
                                       if (camera)
                                       {
                                           camera->Release();
                                       }
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::INTERNAL_THREAD_TERMINATED;
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   return;
                               }

                               case states::WAITING_ON_STREAM_STARTED:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       camera->start_streaming();
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   m_is_streaming = camera->is_streaming();
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }

                               case states::WAITING_ON_STREAM_STOPPED:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       camera->stop_streaming();
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   m_is_streaming = camera->is_streaming();
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }

                               case states::WAITING_ON_STREAM_PARAM_READ:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       auto info = camera->get_source_parameter(stream_param_to_source_reader_param(m_camera_parameter_arg).value());
                                       m_last_param_info.default_value = info.default_value;
                                       m_last_param_info.step = info.step;
                                       m_last_param_info.min = info.min;
                                       m_last_param_info.max = info.max;
                                       m_last_param_info.is_supported = info.is_supported;
                                       m_last_param_value = info.value;
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }

                               case states::WAITING_ON_STREAM_PARAM_AUTO_READ:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       m_last_auto_param_is_automatic = camera->get_source_parameter_auto_mode(stream_auto_param_to_source_reader_auto_param(m_camera_auto_parameter_arg).value());
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }

                               case states::WAITING_ON_STREAM_PARAM_UPDATED:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       camera->set_source_parameter(stream_param_to_source_reader_param(m_camera_parameter_arg).value(), m_last_param_value);
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }

                               case states::WAITING_ON_STREAM_PARAM_AUTO_UPDATED:
                               {
                                   try
                                   {
                                       m_last_exception = nullptr;
                                       if (!camera)
                                       {
                                           throw std::runtime_error("The internal Camera-Object is no longer valid.");
                                       }
                                       camera->set_source_parameter_auto_mode(stream_auto_param_to_source_reader_auto_param(m_camera_auto_parameter_arg).value(), m_last_auto_param_is_automatic);
                                   }
                                   catch (winrt::hresult_error const &ex)
                                   {
                                       m_last_exception = convert_hresult_exception(ex);
                                   }
                                   catch (...)
                                   {
                                       m_last_exception = std::current_exception();
                                   }
                                   m_last_state = states::NOTHING_PENDING;
                                   lk.unlock();
                                   m_notifier.notify_one();
                                   break;
                               }
                               } // end switch
                           } // end while
                       }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_INITIALIZED; });

    if (m_last_exception)
    {
        std::rethrow_exception(m_last_exception);
    }
}

Stream::~Stream()
{

    std::unique_lock<std::mutex> lk(m_mtx);
    if (m_last_state != states::WAITING_ON_INITIALIZED)
    {
        // instruct thread to stop
        m_last_state = states::WAITING_ON_DEINITIALIZED;
    }
    lk.unlock();
    m_notifier.notify_one();
    // wait on future to return
    m_ftr.wait();
}

bool Stream::capture_frame(cv::Mat &destination, std::chrono::milliseconds timeout)
{

    {
        // check for an exception and that we are streaming
        std::scoped_lock lk(m_mtx);
        throw_if_internal_thread_terminated();
        if (m_last_exception)
        {
            std::rethrow_exception(m_last_exception);
        }
        if (!m_is_streaming)
        {
            throw std::runtime_error("The stream is not currently capturing.");
        }
    }

    std::unique_lock<std::mutex> lk(m_image_mtx);
    auto status = m_image_notifier.wait_for(lk, timeout);

    if (m_image_exception)
    {
        std::rethrow_exception(m_image_exception);
    }

    if (status == std::cv_status::timeout)
    {
        return true;
    }

    if (destination.size() != cv::Size(m_cols, m_rows))
    {
        destination = cv::Mat(m_rows, m_cols, CV_8UC4);
    }
    cv::swap(m_current_mat, destination);

    return false;
}

void Stream::start_stream()
{
    // create new scope, request stream start
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        // signal to start streaming
        m_last_state = states::WAITING_ON_STREAM_STARTED;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_STARTED; });
    if (m_last_exception)
    {
        std::rethrow_exception(m_last_exception);
    }
}

void Stream::stop_stream()
{
    // create new scope, request stream stop
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        // signal to stop streaming
        m_last_state = states::WAITING_ON_STREAM_STOPPED;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_STOPPED; });
    if (m_last_exception)
    {
        std::rethrow_exception(m_last_exception);
    }
}

void Stream::get_camera_parameter_info(Stream::camera_parameters param, Stream::param_info_t *info)
{
    info->is_supported = true;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        m_last_state = states::WAITING_ON_STREAM_PARAM_READ;
        m_camera_parameter_arg = param;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_READ; });

    if (m_last_exception)
    {
        info->is_supported = false;
    }
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
        throw_if_internal_thread_terminated();
        m_last_state = states::WAITING_ON_STREAM_PARAM_READ;
        m_camera_parameter_arg = param;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_READ; });

    if (m_last_exception)
    {
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }
    value = m_last_param_value;
    return value;
}

void Stream::set_camera_parameter(Stream::camera_parameters param, int32_t value)
{
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        m_last_state = states::WAITING_ON_STREAM_PARAM_UPDATED;
        m_camera_parameter_arg = param;
        m_last_param_value = value;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_UPDATED; });

    if (m_last_exception)
    {
        throw Stream::param_error{param, "Camera parameter %s not supported."};
    }
}

bool Stream::get_camera_auto_mode(Stream::camera_auto_parameters param)
{
    bool is_auto = false;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        m_last_state = states::WAITING_ON_STREAM_PARAM_AUTO_UPDATED;
        m_camera_auto_parameter_arg = param;
    }

    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_AUTO_UPDATED; });

    if (m_last_exception)
    {
        throw Stream::auto_param_error{param, "Automatic/Manual control of parameter %s not supported."};
    }
    is_auto = m_last_auto_param_is_automatic;
    return is_auto;
}

void Stream::set_camera_auto_mode(camera_auto_parameters param, bool automatic)
{
    bool is_auto = false;
    // create new scope, request param
    {
        std::lock_guard lk(m_mtx);
        throw_if_internal_thread_terminated();
        m_last_state = states::WAITING_ON_STREAM_PARAM_AUTO_UPDATED;
        m_camera_auto_parameter_arg = param;
        m_last_auto_param_is_automatic = automatic;
    }

    m_notifier.notify_one();

    // wait for result
    std::unique_lock<std::mutex> lk(m_mtx);
    m_notifier.wait(lk, [&]
                    { return m_last_state != states::WAITING_ON_STREAM_PARAM_AUTO_UPDATED; });

    if (m_last_exception)
    {
        throw Stream::auto_param_error{param, "Automatic/Manual control of parameter %s not supported."};
    }
}

void Stream::throw_if_internal_thread_terminated() const
{
    if (m_last_state == states::INTERNAL_THREAD_TERMINATED)
    {
        throw std::runtime_error("The internal camera control thread had terminated unexpectedly.");
    }
}