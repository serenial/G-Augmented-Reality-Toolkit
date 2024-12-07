#include <stdexcept>
#include <memory>
#include <future>
#include <thread>
#include <chrono>
#include <winrt/base.h>
#include <mferror.h>
#include <vector>

#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"
#include "g_ar_toolkit/capture/win/utils.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;


// Use a thread to manage all Context Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization
Context::Context() : m_last_state(states::STARTING),
                           m_last_error(errors::NO_ERR),
                           m_ftr(std::async(std::launch::async,
                                          [&]()
                                          {
                                              bool comstartup_ok = false;
                                              bool mfstartup_ok = false;

                                              HRESULT hr = S_OK;
                                              // startup resources

                                              // init this thread for working with COM components
                                              hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

                                              {
                                                  // new lock scope
                                                  std::lock_guard lk(m_mtx);
                                                  if (hr != S_OK && hr != S_FALSE)
                                                  {
                                                      m_last_error = errors::COM_INIT_ERR;
                                                      goto done;
                                                  }
                                                  else
                                                  {
                                                      comstartup_ok = true;
                                                  }

                                                  // MFSTARTUP
                                                  hr = MFStartup(MF_VERSION);

                                                  switch (hr)
                                                  {
                                                  case MF_E_BAD_STARTUP_VERSION:
                                                      m_last_error = errors::MF_STARTUP_BAD_VER;
                                                      break;
                                                  case S_OK:
                                                      mfstartup_ok = true;
                                                      break;
                                                  default:
                                                      m_last_error = errors::MF_STARTUP_OTHER_ERR;
                                                  }
                                                  // update state
                                                  m_last_state = m_last_error == errors::NO_ERR ? states::WAITING_ON_ACTION : states::STOPPING;
                                              }
                                              // setup done
                                              m_notifier.notify_one();

                                              // handle setup fail
                                              if (m_last_error != errors::NO_ERR)
                                              {
                                                  goto done;
                                              }

                                              // main thread loop
                                              // check for external changes to last_state
                                              while (1)
                                              {
                                                  std::unique_lock<std::mutex> lk(m_mtx);
                                                  // poll to see if the state has changed - wait 50ms
                                                  m_notifier.wait(lk, [&]
                                                                { return m_last_state != states::WAITING_ON_ACTION; });
                                                  switch (m_last_state)
                                                  {
                                                  case states::STOPPING:
                                                      lk.unlock();
                                                      goto done;
                                                  case states::LISTING_DEVICES:
                                                      // update the last device enum
                                                      try
                                                      {
                                                          update_last_device_enumeration();
                                                          m_last_error = errors::NO_ERR;
                                                      }
                                                      catch (winrt::hresult_error &e)
                                                      {
                                                          m_last_error = errors::DEVICE_ENUM_ERROR;
                                                      }
                                                      // update flags and signal completion
                                                      m_last_state = states::WAITING_ON_ACTION;
                                                      lk.unlock();
                                                      m_notifier.notify_one();
                                                      break;
                                                  default:
                                                      lk.unlock();
                                                      break;
                                                  }
                                              }

                                          done:

                                              if (mfstartup_ok)
                                              {
                                                  MFShutdown();
                                              }
                                              if (comstartup_ok)
                                              {
                                                  void CoUninitialize();
                                              }

                                              // set state to STOPPED
                                              {
                                                  std::lock_guard lk(m_mtx);
                                                  m_last_state = states::STOPPED;
                                              }
                                              m_notifier.notify_one();

                                              return hr;
                                          }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(m_mtx);
    m_notifier.wait(lk, [&]
                  { return m_last_state != states::STARTING; });

    if (m_last_state != states::WAITING_ON_ACTION)
    {
        const char *error_msg = "An unknown error occured whilst initializing the Context Process.";

        switch (m_last_error)
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

Context::~Context()
{
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        if (m_last_state != states::STOPPED)
        {
            // instruct thread to stop
            m_last_state = states::STOPPING;
            lk.unlock();
            m_notifier.notify_one();
        }
        else
        {
            lk.unlock();
        }
        // wait on future to return
        m_ftr.wait();
    }
}

void Context::enumerate_devices(std::vector<device_info_t> &devices)
{
    // "request" device list
    {
        std::lock_guard lk(m_mtx);
        m_last_state = states::LISTING_DEVICES;
    }
    m_notifier.notify_one();

    // wait for result
    std::unique_lock lk(m_mtx);
    m_notifier.wait(lk, [&]
                  { return m_last_state != states::LISTING_DEVICES; });

    // check error and copy result;
    if (m_last_error == errors::DEVICE_ENUM_ERROR)
    {
        lk.unlock();
        throw std::runtime_error("A error occurred whilst attempting to enumerate devices");
    }
    // copy last enum into devices
    devices = m_last_enumeration;
    // release mutex
    lk.unlock();
}

void Context::update_last_device_enumeration()
{
    winrt::com_ptr<IMFAttributes> spDeviceAttributes;
    UINT32 device_count = 0;

    // create attribute store
    winrt::check_hresult(MFCreateAttributes(spDeviceAttributes.put(), 1));
    // Request video capture devices.
    winrt::check_hresult(spDeviceAttributes->SetGUID(
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID));

    // enumerate devices
    IMFActivate **ppDevices = NULL;
    winrt::check_hresult(MFEnumDeviceSources(spDeviceAttributes.get(), &ppDevices, &device_count));
    // wrap ppDevices into unique pointer
    std::unique_ptr<IMFActivate **, CoTaskMemFreeDeleter<IMFActivate **>> spppDevices(&ppDevices);

    m_last_enumeration.clear();
    m_last_enumeration.reserve(device_count);

    // for each device
    for (UINT i = 0; i < device_count; i++)
    {
        winrt::com_ptr<IMFMediaSource> spMediaSource;
        winrt::com_ptr<IMFSourceReader> spSourceReader;
        device_info_t device_info;

        UINT32 property_length;
        // populate device_id
        winrt::check_hresult((*spppDevices)[i]->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &property_length));
        std::wstring property(property_length + 1, '\0');
        winrt::check_hresult((*spppDevices)[i]->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &property[0], property_length + 1, NULL));
        property.resize(property_length);
        device_info.device_id = winrt::to_string(property);

        // populate device_name
        winrt::check_hresult((*spppDevices)[i]->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &property_length));
        property.resize(property_length + 1);
        winrt::check_hresult((*spppDevices)[i]->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &property[0], property_length + 1, NULL));
        property.resize(property_length);
        device_info.device_name = winrt::to_string(property);

        // activate device and create reader to check the stream options
        winrt::check_hresult((*spppDevices)[i]->ActivateObject(IID_PPV_ARGS(spMediaSource.put())));

        winrt::check_hresult(MFCreateSourceReaderFromMediaSource(
            spMediaSource.get(),
            NULL,
            spSourceReader.put()));

        DWORD dwMediaTypeIndex = 0;

        // loop over until out of media types
        HRESULT hr = S_OK;
        while (hr == S_OK)
        {
            Stream::stream_type_t format;
            winrt::com_ptr<IMFMediaType> spMediaType;
            hr = spSourceReader->GetNativeMediaType(0, dwMediaTypeIndex, spMediaType.put());
            if (hr == MF_E_NO_MORE_TYPES)
            {
                // out of types
                hr = S_OK;
                break;
            }
            else if (hr == S_OK)
            {
                // get the format info
                if (spMediaType != NULL)
                {
                    // get format details
                    winrt::check_hresult(MFGetAttributeSize(spMediaType.get(), MF_MT_FRAME_SIZE, &format.width, &format.height));
                    winrt::check_hresult(MFGetAttributeRatio(spMediaType.get(), MF_MT_FRAME_RATE, &format.fps_numerator, &format.fps_denominator));

                    device_info.supported_formats.push_back(format);
                }
            }
            ++dwMediaTypeIndex;
        }

        m_last_enumeration.push_back(device_info);
    }
}

Stream *Context::open_stream(std::string_view device_id, Stream::stream_type_t stream_type){
    // just return a new stream
    return new Stream(device_id, stream_type);
}