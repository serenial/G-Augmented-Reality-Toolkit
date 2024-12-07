// Adapted from https://github.com/ms-iot/ros_msft_camera  - Copyright (C) Microsoft Corporation. All rights reserved.

// Released Under the MIT Licence
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

#pragma once

#include <string_view>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <Shlwapi.h>
#include <Strmif.h>
#include <ks.h>
#include <Ksmedia.h>

#include <winrt\base.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Devices.Enumeration.h>
#include <winrt\Windows.System.Threading.h>

namespace g_ar_toolkit
{
    namespace capture
    {
        class SourceReader : public IMFSourceReaderCallback
        {
        public:
            // force calling create
            using sample_handler_t = winrt::delegate<winrt::hresult_error, IMFSample *>;
            static SourceReader *create(std::string_view device_id, int32_t width, int32_t height, float frameRate, sample_handler_t sample_handler);

            // IUnknown methods
            STDMETHODIMP QueryInterface(REFIID iid, void **ppv);
            STDMETHODIMP_(ULONG)
            AddRef();
            STDMETHODIMP_(ULONG)
            Release();
            winrt::event_token AddSampleHandler(winrt::delegate<winrt::hresult_error, winrt::hstring, IMFSample *> handler);

            // IMFSourceReaderCallback Methods
            STDMETHODIMP OnEvent(DWORD dwStreamIndex, IMFMediaEvent *mediaEvt);
            STDMETHODIMP OnFlush(DWORD);
            STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample);

            void start_streaming();
            void stop_streaming();

            // parameter control
            struct parameter_values_t
            {
                bool is_supported;
                long min, max, step, default_value, value;
            };

            enum class parameters
            {
                EXPOSURE,
                FOCUS,
                ZOOM,
                WHITE_BALANCE_TEMPERATURE,
                BRIGHTNESS,
                CONTRAST,
                SATURATION,
                GAMMA,
                HUE,
                SHARPNESS,
                BACKLIGHT_COMPENSATION,
                POWER_LINE_FREQUENCY,
                GAIN
            };

            enum class auto_parameters
            {
                AUTO_EXPOSURE,
                AUTO_FOCUS,
                AUTO_WHITE_BALANCE,
                AUTO_GAIN
            };

            parameter_values_t get_source_parameter(parameters);
            void set_source_parameter(parameters, int32_t);
            bool get_source_parameter_auto_mode(auto_parameters);
            void set_source_parameter_auto_mode(auto_parameters, bool);

            template <class T>
            void source_reader_parameter_get(int32_t parameter, parameter_values_t &parameter_values)
            {
                // get the relevant interface for querying the parameter
                winrt::com_ptr<T> control;

                check_hresult(m_source_reader->GetServiceForStream(MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&control)));

                long parameter_flag;

                check_hresult(control->Get(parameter, &parameter_values.value, &parameter_flag));
                // query range, step and default
                check_hresult(control->GetRange(parameter, &parameter_values.min, &parameter_values.max, &parameter_values.step, &parameter_values.default_value, &parameter_flag));
            }

            template <class T>
            void source_reader_parameter_set(int32_t parameter, int32_t value, int32_t parameter_flag)
            {
                // get the relevant interface for querying the parameter
                winrt::com_ptr<T> control;

                check_hresult(m_source_reader->GetServiceForStream(MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&control)));

                check_hresult(control->Set(parameter, value, parameter_flag));
            }

            template <class T>
            long source_reader_parameter_flag_get(int32_t parameter)
            {
                // get the relevant interface for querying the parameter
                winrt::com_ptr<T> control;

                check_hresult(m_source_reader->GetServiceForStream(MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&control)));

                long value, parameter_flag;
                check_hresult(control->Get(parameter, &value, &parameter_flag));

                return parameter_flag;
            }

            template <class T>
            void source_reader_parameter_flag_set(int32_t parameter, long parameter_flag)
            {
                // get the relevant interface for querying the parameter
                winrt::com_ptr<T> control;

                check_hresult(m_source_reader->GetServiceForStream(MF_SOURCE_READER_MEDIASOURCE, GUID_NULL, IID_PPV_ARGS(&control)));

                long value, existing_flag;
                check_hresult(control->Get(parameter, &value, &existing_flag));

                if(parameter_flag != existing_flag){
                    check_hresult(control->Set(parameter, value, parameter_flag));
                }
            }

        private:
            // make constructor/destructor private to force usage through create and Release
            ~SourceReader();
            SourceReader(std::string_view device_id, int32_t width, int32_t height, float frameRate, sample_handler_t on_sample);
            bool find_matching_media_type(IMFMediaType **matching_media_type, int32_t width, int32_t height, float frameRate, GUID preferred_video_sub_type);

            winrt::com_ptr<IMFMediaSource> m_media_source;
            winrt::com_ptr<IMFSourceReader> m_source_reader;
            UINT32 m_width, m_height;
            UINT32 m_source_reader_flags;
            winrt::event<winrt::delegate<>> m_config_event;
            winrt::Windows::Foundation::Collections::IVector<winrt::event_token> m_config_event_token_list;
            winrt::event<sample_handler_t> m_capture_callback_event;
            long m_ref_count;
            CRITICAL_SECTION m_critsec;
            bool m_streaming_started;
            winrt::slim_mutex m_api_guard_mutex;
            winrt::slim_mutex m_sample_handler_mutex;
        };
    }
}