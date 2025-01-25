// Adapted from https://github.com/ms-iot/ros_msft_camera  - Copyright (C) Microsoft Corporation. All rights reserved.

// Released under the MIT Licence
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <mutex>

#include "g_ar_toolkit/capture/win/source_reader.hpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::System::Threading;

using namespace g_ar_toolkit;
using namespace capture;

SourceReader::SourceReader(std::string_view device_id, int32_t width, int32_t height, float frameRate, SourceReader::sample_handler_t on_sample)
    : m_ref_count(1),
      m_source_reader_flags(0),
      m_height(height),
      m_width(width),
      m_streaming_started(false)
{
    InitializeCriticalSection(&m_critsec);
    m_config_event_token_list = single_threaded_vector<winrt::event_token>();

    winrt::com_ptr<IMFAttributes> attributes;
    winrt::com_ptr<IMFAttributes> source_reader_attributes;
    winrt::com_ptr<IMFSensorGroup> sensor_grp;
    winrt::com_ptr<IMFSensorDevice> sensor_device;

    // init COM stuff
    check_hresult(CoInitialize(NULL));
    check_hresult(MFStartup(MF_VERSION));

    // setup media source

    check_hresult(MFCreateSensorGroup(winrt::to_hstring(device_id).c_str(), sensor_grp.put()));
    check_hresult(sensor_grp->GetSensorDevice(0, sensor_device.put()));
    check_hresult(sensor_device->SetSensorDeviceMode(MFSensorDeviceMode::MFSensorDeviceMode_Controller));
    check_hresult(sensor_grp->CreateMediaSource(m_media_source.put()));
    check_hresult(MFCreateAttributes(source_reader_attributes.put(), 3));
    check_hresult(source_reader_attributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE));
    check_hresult(source_reader_attributes->SetUINT32(MF_LOW_LATENCY, TRUE));
    check_hresult(source_reader_attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this));
    check_hresult(MFCreateSourceReaderFromMediaSource(m_media_source.get(), source_reader_attributes.get(), m_source_reader.put()));

    // add on_sample handler
    m_capture_callback_event.add(on_sample);

    // configure stream
    std::lock_guard g(m_api_guard_mutex);
    bool status = true;
    std::condition_variable eventCompletion;
    std::mutex m;
    std::unique_lock<std::mutex> ul(m);
    auto config_handler = [&]()
    {
        HRESULT hr = S_OK;
        winrt::com_ptr<IMFMediaType> media_type;
        UINT32 frame_rate_num, frame_rate_den;
        int idx = 0;

        if (!find_matching_media_type(media_type.put(), width, height, frameRate, MFVideoFormat_ARGB32))
        {
            if (!find_matching_media_type(media_type.put(), width, height, frameRate, GUID_NULL))
            {
                if (!find_matching_media_type(media_type.put(), width, height, 0, GUID_NULL))
                {
                    status = false;
                }
            }
            if (status)
            {
                DWORD flags;
                check_hresult(m_source_reader.as<IMFSourceReaderEx>()->SetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, media_type.get(), &flags));
                check_hresult(media_type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32));
                check_hresult(m_source_reader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, media_type.get()));
                m_width = width;
                m_height = height;
            }
        }
        else
        {
            DWORD flags = 0;
            GUID sub_type;
            HRESULT hr = m_source_reader.as<IMFSourceReaderEx>()->SetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, media_type.get(), &flags);

            if (SUCCEEDED(hr))
            {
                m_width = width;
                m_height = height;
            }
        }
        eventCompletion.notify_one();
    };
    if (m_streaming_started)
    {
        EnterCriticalSection(&m_critsec);
        // post a delegate to be processed by the sample processing thread, so the order of mediatype change is maintained
        m_config_event_token_list.Append(m_config_event.add(config_handler));
        LeaveCriticalSection(&m_critsec);
        check_hresult(m_source_reader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM));
        eventCompletion.wait(ul);
        check_hresult(m_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, m_source_reader_flags, NULL, NULL, NULL, NULL));
    }
    else
    {
        config_handler();
    }
}

void SourceReader::stop_streaming()
{
    std::lock_guard g(m_api_guard_mutex);
    if (!m_streaming_started)
    {
        return;
    }

    std::condition_variable event_completion;
    std::mutex m;
    std::unique_lock ul(m);

    EnterCriticalSection(&m_critsec);
    m_config_event_token_list.Append(m_config_event.add([&]()
                                                        {
            m_streaming_started = false;
            try{
                check_hresult(m_source_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, FALSE));
            }
            catch(...){
                event_completion.notify_one();
                std::rethrow_exception(std::current_exception());
            }
            event_completion.notify_one(); }));
    LeaveCriticalSection(&m_critsec);

    try{
        check_hresult(m_source_reader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM));
    }
    catch(...){
        // wait for the config-stop event to complete
        event_completion.wait(ul);
        ul.unlock();
        std::rethrow_exception(std::current_exception());
    }

    // wait for the config-stop event to complete
    event_completion.wait(ul);
    ul.unlock();

    m_capture_callback_event(callback_events::END_OF_STREAM, nullptr);
}

void SourceReader::start_streaming()
{
    std::lock_guard g(m_api_guard_mutex);
    if (m_streaming_started)
    {
        return;
    }

    winrt::com_ptr<IMFMediaType> spMT;
    GUID subtype;

    check_hresult(m_source_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE));
    check_hresult(m_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, m_source_reader_flags, NULL, NULL, NULL, NULL));
    m_streaming_started = true;
}

HRESULT SourceReader::OnReadSample(
    HRESULT hr_status,
    DWORD dw_atream_index,
    DWORD dw_stream_flags,
    LONGLONG ll_timestamp,
    IMFSample *p_sample)
{
    std::lock_guard g(m_sample_handler_mutex);
    try
    {
        if (SUCCEEDED(hr_status))
        {
            if (!p_sample && !dw_stream_flags)
            {
                // Drain completed
                winrt::com_ptr<IMFMediaType> mt;
                GUID subtype;
                m_source_reader_flags = 0;
                check_hresult(m_source_reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, mt.put()));

                check_hresult(mt->GetGUID(MF_MT_SUBTYPE, &subtype));
                if (!IsEqualGUID(subtype, MFVideoFormat_ARGB32))
                {
                    check_hresult(mt->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32));
                    check_hresult(m_source_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, mt.get()));
                }
                check_hresult(MFGetAttributeSize(mt.get(), MF_MT_FRAME_SIZE, &m_width, &m_height));
            }
        }
        else
        {
            throw_hresult(hr_status);
        }
        if (MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED & dw_stream_flags)
        {
            winrt::com_ptr<IMFMediaType> mt;
            check_hresult(m_source_reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, mt.put()));
            check_hresult(MFGetAttributeSize(mt.get(), MF_MT_FRAME_SIZE, &m_width, &m_height));
        }
        if (MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED & dw_stream_flags)
        {
            winrt::com_ptr<IMFMediaType> mt;
            check_hresult(m_source_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, mt.put()));
            check_hresult(MFGetAttributeSize(mt.get(), MF_MT_FRAME_SIZE, &m_width, &m_height));

            m_source_reader_flags = MF_SOURCE_READER_CONTROLF_DRAIN;
        }
        if (MF_SOURCE_READERF_ENDOFSTREAM & dw_stream_flags)
        {
            m_capture_callback_event(callback_events::END_OF_STREAM, nullptr);
        }
    }
    catch (hresult_error const &ex)
    {
        m_capture_callback_event(callback_events::EXCEPTION, nullptr);
    }
    if (m_streaming_started)
    {
        if (SUCCEEDED(hr_status) && p_sample)
        {
            m_capture_callback_event(callback_events::SAMPLE, p_sample);
        }

        HRESULT hr = m_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, m_source_reader_flags, NULL, NULL, NULL, NULL);
        if (hr == MF_E_NOTACCEPTING)
        {
            m_capture_callback_event(callback_events::NOT_ACCEPTING, nullptr);
        }
        else if (FAILED(hr))
        {
            m_capture_callback_event(callback_events::EXCEPTION, nullptr);
        }
    }
    return S_OK;
}

bool SourceReader::find_matching_media_type(IMFMediaType **matching_media_type, int32_t width, int32_t height, float frameRate, GUID preferred_video_sub_type)
{
    winrt::com_ptr<IMFMediaType> media_type;
    int idx = 0;
    UINT32 frame_rate_num, frame_rate_den;
    bool match = true;
    GUID sub_type = GUID_NULL;
    check_pointer(matching_media_type);
    while (S_OK == m_source_reader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, idx, media_type.put()))
    {
        check_hresult(MFGetAttributeSize(media_type.get(), MF_MT_FRAME_SIZE, &m_width, &m_height));
        check_hresult(MFGetAttributeRatio(media_type.get(), MF_MT_FRAME_RATE, &frame_rate_num, &frame_rate_den));
        check_hresult(media_type->GetGUID(MF_MT_SUBTYPE, &sub_type));
        float rate = ((float)frame_rate_num / (float)frame_rate_den);
        match = ((height == 0) || (width == 0) || ((m_width == width) && (height == m_height)));
        match = match && (((int)rate == (int)frameRate) || (frameRate == 0));
        match = match && (IsEqualGUID(preferred_video_sub_type, sub_type) || IsEqualGUID(preferred_video_sub_type, GUID_NULL));
        if (match)
        {
            break;
        }
        media_type = nullptr; // reset com_ptr
        idx++;
    }
    media_type.copy_to(matching_media_type);
    return match;
}

SourceReader *SourceReader::create(std::string_view device_id, int32_t width, int32_t height, float frame_rate, sample_handler_t sample_handler)
{
    return new SourceReader(device_id, width, height, frame_rate, sample_handler);
}

// IUnknown methods
STDMETHODIMP SourceReader::QueryInterface(REFIID iid, void **ppv)
{
    static const QITAB qit[] =
        {
            QITABENT(SourceReader, IMFSourceReaderCallback),
            {0},
        };
    return QISearch(this, qit, iid, ppv);
}

STDMETHODIMP_(ULONG)
SourceReader::AddRef()
{
    return InterlockedIncrement(&m_ref_count);
}

STDMETHODIMP_(ULONG)
SourceReader::Release()
{
    ULONG uCount = InterlockedDecrement(&m_ref_count);
    if (uCount == 0)
    {
        delete this;
    }
    return uCount;
}

STDMETHODIMP SourceReader::OnEvent(DWORD dwStreamIndex, IMFMediaEvent *mediaEvt)
{
    MediaEventType evt;
    mediaEvt->GetType(&evt);

    return S_OK;
}

STDMETHODIMP SourceReader::OnFlush(DWORD)
{
    EnterCriticalSection(&m_critsec);
    m_config_event();
    for (auto token : m_config_event_token_list)
    {
        m_config_event.remove(token);
    }
    LeaveCriticalSection(&m_critsec);
    return S_OK;
}

SourceReader::parameter_values_t SourceReader::get_source_parameter(SourceReader::parameters parameter_name)
{
    parameter_values_t param_values;
    std::lock_guard g(m_api_guard_mutex);
    switch (parameter_name)
    {
    case parameters::EXPOSURE:
        source_reader_parameter_get<IAMCameraControl>(CameraControl_Exposure, param_values);
        break;
    case parameters::FOCUS:
        source_reader_parameter_get<IAMCameraControl>(CameraControl_Focus, param_values);
        break;
    case parameters::ZOOM:
        source_reader_parameter_get<IAMCameraControl>(CameraControl_Zoom, param_values);
        break;
    case parameters::WHITE_BALANCE_TEMPERATURE:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_WhiteBalance, param_values);
        break;
    case parameters::BRIGHTNESS:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Brightness, param_values);
        break;
    case parameters::CONTRAST:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Contrast, param_values);
        break;
    case parameters::SATURATION:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Saturation, param_values);
        break;
    case parameters::GAMMA:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Gamma, param_values);
        break;
    case parameters::HUE:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Hue, param_values);
        break;
    case parameters::SHARPNESS:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Sharpness, param_values);
        break;
    case parameters::BACKLIGHT_COMPENSATION:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_BacklightCompensation, param_values);
        break;
    case parameters::POWER_LINE_FREQUENCY:
        source_reader_parameter_get<IAMVideoProcAmp>(KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, param_values);
    case parameters::GAIN:
        source_reader_parameter_get<IAMVideoProcAmp>(VideoProcAmp_Gain, param_values);
        break;
    }

    return param_values;
}

void SourceReader::set_source_parameter(SourceReader::parameters parameter_name, int32_t value)
{
    std::lock_guard g(m_api_guard_mutex);
    switch (parameter_name)
    {
    case parameters::EXPOSURE:
        source_reader_parameter_set<IAMCameraControl>(CameraControl_Exposure, value, CameraControl_Flags_Manual);
        break;
    case parameters::FOCUS:
        source_reader_parameter_set<IAMCameraControl>(CameraControl_Focus, value, CameraControl_Flags_Manual);
        break;
    case parameters::ZOOM:
        source_reader_parameter_set<IAMCameraControl>(CameraControl_Zoom, value, CameraControl_Flags_Manual);
        break;
    case parameters::WHITE_BALANCE_TEMPERATURE:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_WhiteBalance, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::BRIGHTNESS:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Brightness, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::CONTRAST:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Contrast, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::SATURATION:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Saturation, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::GAMMA:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Gamma, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::HUE:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Hue, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::SHARPNESS:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Sharpness, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::BACKLIGHT_COMPENSATION:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_BacklightCompensation, value, VideoProcAmp_Flags_Manual);
        break;
    case parameters::POWER_LINE_FREQUENCY:
        source_reader_parameter_set<IAMVideoProcAmp>(KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, value, VideoProcAmp_Flags_Manual);
    case parameters::GAIN:
        source_reader_parameter_set<IAMVideoProcAmp>(VideoProcAmp_Gain, value, VideoProcAmp_Flags_Manual);
        break;
    }
}

bool SourceReader::get_source_parameter_auto_mode(SourceReader::auto_parameters auto_param)
{
    switch (auto_param)
    {
    case auto_parameters::AUTO_EXPOSURE:
        return source_reader_parameter_flag_get<IAMCameraControl>(CameraControl_Exposure) == CameraControl_Flags_Auto;
    case auto_parameters::AUTO_FOCUS:
        return source_reader_parameter_flag_get<IAMCameraControl>(CameraControl_Focus) == CameraControl_Flags_Auto;
    case auto_parameters::AUTO_GAIN:
        return source_reader_parameter_flag_get<IAMVideoProcAmp>(VideoProcAmp_Gain) == VideoProcAmp_Flags_Auto;
    case auto_parameters::AUTO_WHITE_BALANCE:
        return source_reader_parameter_flag_get<IAMVideoProcAmp>(VideoProcAmp_WhiteBalance) == VideoProcAmp_Flags_Auto;
    }
    return false;
}

void SourceReader::set_source_parameter_auto_mode(SourceReader::auto_parameters auto_param, bool automatic)
{
    switch (auto_param)
    {
    case auto_parameters::AUTO_EXPOSURE:
        source_reader_parameter_flag_set<IAMCameraControl>(CameraControl_Exposure, automatic? CameraControl_Flags_Auto : CameraControl_Flags_Manual);
        break;
    case auto_parameters::AUTO_FOCUS:
        source_reader_parameter_flag_set<IAMCameraControl>(CameraControl_Focus, automatic? CameraControl_Flags_Auto : CameraControl_Flags_Manual);
        break;
    case auto_parameters::AUTO_GAIN:
        source_reader_parameter_flag_set<IAMVideoProcAmp>(VideoProcAmp_Gain, automatic? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual);
        break;
    case auto_parameters::AUTO_WHITE_BALANCE:
        source_reader_parameter_flag_set<IAMVideoProcAmp>(VideoProcAmp_WhiteBalance, automatic? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual);
        break;
    }
}

SourceReader::~SourceReader(){
    // clean-up the COM and wmf components
     MFShutdown();                                 
    void CoUninitialize();
}