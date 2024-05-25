#include <stdexcept>
#include <memory>
#include <future>
#include <thread>
#include <chrono>
#include <winrt/base.h>
#include <mferror.h>
#include <vector>

#include "g_ar_toolkit/win/context_wmf.hpp"
#include "g_ar_toolkit/win/context_wmf_formats.hpp"
#include "g_ar_toolkit/capture/stream.hpp"

using namespace g_ar_toolkit;
using namespace capture;
using namespace std::chrono_literals;

Context *capture::create_platform_context()
{
    return new ContextWMF;
}

// Use a thread to manage all Context Operations as this provides more control over
// COM and Media-Foundation Component Lifetime
// Specify all the Context Functionality inside a lambda which is controlled via
// mutex/conditional_variable synchronization
ContextWMF::ContextWMF() : Context(),
                           last_state(states::STARTING),
                           last_error(errors::NO_ERR),
                           format_lookup(
                               [&]()
                               {
                                   std::unordered_map<GUID, format_item_t, GUIDHash> lookup;
                                   for (uint32_t i = 0; i < std::size(formats_guid_and_names); i++)
                                   {
                                       format_item_t value{i, formats_guid_and_names[i].second};
                                       lookup.emplace(formats_guid_and_names[i].first, value);
                                   }
                                   return lookup;
                               }()),
                           ftr(std::async(std::launch::async,
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
                                                  std::lock_guard lk(mtx);
                                                  if (hr != S_OK && hr != S_FALSE)
                                                  {
                                                      last_error = errors::COM_INIT_ERR;
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
                                                      last_error = errors::MF_STARTUP_BAD_VER;
                                                      break;
                                                  case S_OK:
                                                      mfstartup_ok = true;
                                                      break;
                                                  default:
                                                      last_error = errors::MF_STARTUP_OTHER_ERR;
                                                  }
                                                  // update state
                                                  last_state = last_error == errors::NO_ERR ? states::WAITING_ON_ACTION : states::STOPPING;
                                              }
                                              // setup done
                                              notifier.notify_one();

                                              // handle setup fail
                                              if (last_error != errors::NO_ERR)
                                              {
                                                  goto done;
                                              }

                                              // main thread loop
                                              // check for external changes to last_state
                                              while (1)
                                              {
                                                  std::unique_lock<std::mutex> lk(mtx);
                                                  // poll to see if the state has changed - wait 50ms
                                                  notifier.wait(lk, [&]
                                                                { return last_state != states::WAITING_ON_ACTION; });
                                                  switch (last_state)
                                                  {
                                                  case states::STOPPING:
                                                      lk.unlock();
                                                      goto done;
                                                  case states::LISTING_DEVICES:
                                                      // update the last device enum
                                                      try
                                                      {
                                                          update_last_device_enumeration();
                                                          last_error = errors::NO_ERR;
                                                      }
                                                      catch (winrt::hresult_error &e)
                                                      {
                                                          last_error = errors::DEVICE_ENUM_ERROR;
                                                      }
                                                      // update flags and signal completion
                                                      last_state = states::WAITING_ON_ACTION;
                                                      lk.unlock();
                                                      notifier.notify_one();
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
                                                  std::lock_guard lk(mtx);
                                                  last_state = states::STOPPED;
                                              }
                                              notifier.notify_one();

                                              return hr;
                                          }))
{
    // wait on cv to see if the co-thread initialized ok
    std::unique_lock lk(mtx);
    notifier.wait(lk, [&]
                  { return last_state != states::STARTING; });

    if (last_state != states::WAITING_ON_ACTION)
    {
        const char *error_msg = "An unknown error occured whilst initializing the Context Process.";

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

ContextWMF::~ContextWMF()
{
    {
        std::unique_lock<std::mutex> lk(mtx);
        if (last_state != states::STOPPED)
        {
            // instruct thread to stop
            last_state = states::STOPPING;
            lk.unlock();
            notifier.notify_one();
        }
        else
        {
            lk.unlock();
        }
        // wait on future to return
        ftr.wait();
    }
}

void ContextWMF::enumerate_devices(std::vector<device_info_t> &devices)
{
    // "request" device list
    {
        std::lock_guard lk(mtx);
        last_state = states::LISTING_DEVICES;
    }
    notifier.notify_one();

    // wait for result
    std::unique_lock lk(mtx);
    notifier.wait(lk, [&]
                  { return last_state != states::LISTING_DEVICES; });

    // check error and copy result;
    if (last_error == errors::DEVICE_ENUM_ERROR)
    {
        lk.unlock();
        throw std::runtime_error("A error occurred whilst attempting to enumerate devices");
    }
    // copy last enum into devices
    devices = last_enumeration;
    // release mutex
    lk.unlock();
}

void ContextWMF::update_last_device_enumeration()
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

    last_enumeration.clear();
    last_enumeration.reserve(device_count);

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
            stream_type_with_format_t format;
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
                GUID subType;
                if (spMediaType != NULL)
                    // lookup name of format
                    winrt::check_hresult(spMediaType->GetGUID(MF_MT_SUBTYPE, &subType));
                try
                {
                    format.pixel_format = format_lookup.at(subType).index;
                }
                catch (std::out_of_range &e)
                {
                    format.pixel_format = 0;
                }
                // get format details
                winrt::check_hresult(MFGetAttributeSize(spMediaType.get(), MF_MT_FRAME_SIZE, &format.stream_type.width, &format.stream_type.height));
                winrt::check_hresult(MFGetAttributeRatio(spMediaType.get(), MF_MT_FRAME_RATE, &format.stream_type.fps_numerator, &format.stream_type.fps_denominator));

                // push this format into the vector
                device_info.supported_formats.push_back(format);
            }
            ++dwMediaTypeIndex;
        }

        last_enumeration.push_back(device_info);
    }
}

void ContextWMF::list_of_formats(std::vector<format_item_t> &list)
{
    for (auto const &item : format_lookup)
    {
        format_item_t value {item.second.index, item.second.name};
        list.push_back(value);
    }
}