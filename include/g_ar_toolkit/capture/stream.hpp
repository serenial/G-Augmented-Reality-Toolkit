#pragma once

#include <string>
#include <string_view>
#include <chrono>
#include <exception>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#ifdef __linux__
#include "./linux/utils.hpp"
#include "./linux/decoders.hpp"
#endif

#ifdef _WIN32
#include <mutex>
#include <condition_variable>
#include <future>
#include "./win/source_reader.hpp"
#endif

namespace g_ar_toolkit
{
    namespace capture
    {

        class Stream
        {
        public:
            enum class camera_parameters
            {
                EXPOSURE,
                FOCUS,
                ZOOM,
                GAIN,
                WHITE_BALANCE_TEMPERATURE,
                BRIGHTNESS,
                CONTRAST,
                SATURATION,
                GAMMA,
                HUE,
                SHARPNESS,
                BACKLIGHT_COMPENSATION,
                POWER_LINE_FREQUENCY
            };
            enum class camera_auto_parameters
            {
                AUTO_EXPOSURE,
                AUTO_FOCUS,
                AUTO_WHITE_BALANCE,
                AUTO_GAIN
            };
            struct param_info_t
            {
                bool is_supported;
                int32_t min, max, step, default_value;
                param_info_t();
            };

            struct stream_type_t
            {
                uint32_t width, height, fps_numerator, fps_denominator;
            };

            Stream(std::string_view device_id, stream_type_t stream_type);
            ~Stream();
            void start_stream();
            void stop_stream();
            void capture_frame(cv::Mat &, std::chrono::milliseconds);
            void get_camera_parameter_info(camera_parameters, param_info_t *);
            int32_t get_camera_parameter(camera_parameters);
            void set_camera_parameter(camera_parameters, int32_t);
            bool get_camera_auto_mode(camera_auto_parameters);
            void set_camera_auto_mode(camera_auto_parameters, bool);

            class param_error : public std::runtime_error
            {
            private:
                static std::string format_message(camera_parameters, std::string_view);

            public:
                param_error() = delete;
                param_error(camera_parameters, std::string_view message);
            };

            class auto_param_error : public std::runtime_error
            {
            private:
                static std::string format_message(camera_auto_parameters, std::string_view);

            public:
                auto_param_error() = delete;
                auto_param_error(camera_auto_parameters, std::string_view message);
            };

#ifdef __linux__
        private:
            Stream(std::string_view, stream_type_t, std::pair<scoped_file_descriptor, __u32>);
            void enqueue_buffer(int);
            int dequeue_buffer(size_t *n_bytes);
            const stream_type_t m_stream_type;
            const std::string_view m_device_id;
            scoped_file_descriptor m_scoped_fd; // device handle
            const __u32 m_pixel_format;
            const std::unique_ptr<decoder> m_decoder;
            const std::vector<scoped_mmap_buffer> m_buffer_list;
            bool m_is_streaming;
#endif

#ifdef _WIN32
        private:
            enum class states
            {
                STARTING,
                WAITING_ON_ACTION,
                WAITING_ON_STREAM_START,
                WAITING_ON_CAPTURE,
                WAITING_ON_CAPTURE_ACK,
                WAITING_ON_STREAM_STOP,
                WAITING_ON_STREAM_PARAM_GET,
                WAITING_ON_STREAM_PARAM_AUTO_GET,
                WAITING_ON_STREAM_PARAM_SET,
                WAITING_ON_STREAM_PARAM_AUTO_SET,
                STOPPING,
                STOPPED
            };

            std::mutex m_mtx;
            std::condition_variable m_notifier;
            states m_last_state;
            camera_parameters m_camera_parameter_arg;
            camera_auto_parameters m_camera_auto_parameter_arg;
            param_info_t m_last_param_info;
            int32_t m_last_param_value;
            bool m_last_auto_param_is_automatic;
            cv::Mat m_buffer_mat;
            cv::Mat *m_dest_mat_ptr;
            const std::future<void> m_ftr;
            winrt::event_token m_sample_handler_token;
            const uint32_t m_rows, m_cols;
            std::exception_ptr m_last_exception;
#endif

        private:
            // platform specific templates that will only be compiled as required
            template <class T>
            void set_query_struct_id_by_camera_parameter(T &query_struct, Stream::camera_parameters param)
            {
                switch (param)
                {
                case Stream::camera_parameters::EXPOSURE:
                    query_struct.id = V4L2_CID_EXPOSURE_ABSOLUTE;
                    break;
                case Stream::camera_parameters::FOCUS:
                    query_struct.id = V4L2_CID_FOCUS_ABSOLUTE;
                    break;
                case Stream::camera_parameters::ZOOM:
                    query_struct.id = V4L2_CID_ZOOM_ABSOLUTE;
                    break;
                case Stream::camera_parameters::GAIN:
                    query_struct.id = V4L2_CID_GAIN;
                    break;
                case Stream::camera_parameters::WHITE_BALANCE_TEMPERATURE:
                    query_struct.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
                    break;
                case Stream::camera_parameters::BRIGHTNESS:
                    query_struct.id = V4L2_CID_BRIGHTNESS;
                    break;
                case Stream::camera_parameters::CONTRAST:
                    query_struct.id = V4L2_CID_CONTRAST;
                    break;
                case Stream::camera_parameters::SATURATION:
                    query_struct.id = V4L2_CID_SATURATION;
                    break;
                case Stream::camera_parameters::GAMMA:
                    query_struct.id = V4L2_CID_GAMMA;
                    break;
                case Stream::camera_parameters::HUE:
                    query_struct.id = V4L2_CID_HUE;
                    break;
                case Stream::camera_parameters::SHARPNESS:
                    query_struct.id = V4L2_CID_SHARPNESS;
                    break;
                case Stream::camera_parameters::BACKLIGHT_COMPENSATION:
                    query_struct.id = V4L2_CID_BACKLIGHT_COMPENSATION;
                    break;
                case Stream::camera_parameters::POWER_LINE_FREQUENCY:
                    query_struct.id = V4L2_CID_POWER_LINE_FREQUENCY;
                    break;
                default:
                    throw std::invalid_argument("Camera Parameter type invalid.");
                }
            }

            template <class T>
            void set_query_struct_id_by_auto_camera_parameter(T &query_struct, Stream::camera_auto_parameters param)
            {
                switch (param)
                {
                case Stream::camera_auto_parameters::AUTO_EXPOSURE:
                    query_struct.id = V4L2_CID_EXPOSURE_AUTO;
                    break;
                case Stream::camera_auto_parameters::AUTO_FOCUS:
                    query_struct.id = V4L2_CID_FOCUS_AUTO;
                    break;
                case Stream::camera_auto_parameters::AUTO_WHITE_BALANCE:
                    query_struct.id = V4L2_CID_AUTO_WHITE_BALANCE;
                    break;
                case Stream::camera_auto_parameters::AUTO_GAIN:
                    query_struct.id = V4L2_CID_AUTOGAIN;
                    break;
                default:
                    throw std::invalid_argument("Camera Auto Parameter type invalid.");
                }
            }
        };
    }
}