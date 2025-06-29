#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_videoio_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_reader_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t path_string_handle,
        LV_EDVRReferencePtr_t reader_edvr_ref)
    {
        try
        {
            EDVRManagedObject<cv::VideoCapture> cap(reader_edvr_ref, new cv::VideoCapture());

            cap->open(path_string_handle, cv::CAP_FFMPEG);

            if (!cap->isOpened())
            {
                throw std::runtime_error("Unable to open video source.");
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_reader_read_frame(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t reader_edvr_ref,
        LV_EDVRReferencePtr_t img_edvr_ref,
        double position,
        uint8_t position_type)
    {
        try
        {
            EDVRManagedObject<cv::VideoCapture> cap(reader_edvr_ref);

            if (!cap->isOpened())
            {
                throw std::runtime_error("Video source is invalid.");
            }

            bool set_ok = true;
            switch (position_type)
            {
            case 0:
                // set position via index
                set_ok = cap->set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, position);
                break;
            // set position in ms
            case 1:
                set_ok = cap->set(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC, position);
                break;
            }

            if(!set_ok){
                throw std::invalid_argument("Unable to set the position to the specified position.");
            }

            cv::Mat tmp;
            auto ok = cap->read(tmp);

            if (!ok)
            {
                throw std::invalid_argument("Getting the next/specified frame failed.");
            }

            lv_image dst(img_edvr_ref);

            if (dst.is_greyscale())
            {
                switch (tmp.channels())
                {
                case 1:
                    tmp.copyTo(dst);
                    break;
                case 3:
                    dst.ensure_sized_to_match(tmp.size());
                    cv::cvtColor(tmp, dst, cv::COLOR_BGR2GRAY);
                    break;
                case 4:
                    dst.ensure_sized_to_match(tmp.size());
                    cv::cvtColor(tmp, dst, cv::COLOR_BGRA2GRAY);
                    break;
                }
            }
            else
            {
                switch (tmp.channels())
                {
                case 1:
                    dst.ensure_sized_to_match(tmp.size());
                    cv::cvtColor(tmp, dst, cv::COLOR_GRAY2BGRA);
                    break;
                case 3:
                    dst.ensure_sized_to_match(tmp.size());
                    cv::cvtColor(tmp, dst, cv::COLOR_BGR2BGRA);
                    break;
                case 4:
                    tmp.copyTo(dst);
                default:
                    throw std::runtime_error("Returned frame had an unexpected number of channels.");
                }
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_reader_get_info(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t reader_edvr_ref,
        LV_ImageSizePtr_t size,
        double *frame_rate,
        uint64_t *n_frames)
    {
        try
        {

            EDVRManagedObject<cv::VideoCapture> cap(reader_edvr_ref);

            if (!cap->isOpened())
            {
                throw std::runtime_error("Video source is invalid.");
            }

            size->m_height = cap->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT);
            size->m_width = cap->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH);
            *frame_rate = cap->get(cv::VideoCaptureProperties::CAP_PROP_FPS);
            *n_frames = cap->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_reader_get_set_position(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t reader_edvr_ref,
        double* position,
        LV_BooleanPtr_t get,
        LV_BooleanPtr_t frame_index
    )
    {
        try
        {

            EDVRManagedObject<cv::VideoCapture> cap(reader_edvr_ref);

            if (!cap->isOpened())
            {
                throw std::runtime_error("Video source is invalid.");
            }

            if(*get){
                if(*frame_index){
                    *position = cap->get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES);
                }
                else{
                    *position = cap->get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC);
                }
            }
            else{
                bool ok = false;
                if(*frame_index){
                    ok = cap->set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, *position);
                }
                else{
                    ok = cap->set(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC, *position);
                }

                if(!ok){
                    throw std::invalid_argument("Unable to set the position to the specified position.");
                }
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_writer_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t path_string_handle,
        uint8_t fourcc_0,
        uint8_t fourcc_1,
        uint8_t fourcc_2,
        uint8_t fourcc_3,
        LV_EDVRReferencePtr_t writer_edvr_ref,
        LV_ImageSizePtr_t size,
        double fps,
        LV_BooleanPtr_t is_colour,
        int8_t quality,
        LV_BooleanPtr_t use_hw_acceleration
    )
    {
        try
        {
            auto fourcc = cv::VideoWriter::fourcc(fourcc_0,fourcc_1,fourcc_2,fourcc_3);

            std::vector<int> args(4);

            args[0] = cv::VideoWriterProperties::VIDEOWRITER_PROP_IS_COLOR;
            args[1] = *is_colour;
            args[2] = cv::VideoWriterProperties::VIDEOWRITER_PROP_HW_ACCELERATION;
            args[3] = *use_hw_acceleration? cv::VIDEO_ACCELERATION_ANY : cv::VIDEO_ACCELERATION_NONE;

            if(quality>=0){
                args.push_back(cv::VideoWriterProperties::VIDEOWRITER_PROP_QUALITY);
                args.push_back(quality);
            }

            EDVRManagedObject<cv::VideoWriter> writer(writer_edvr_ref, new cv::VideoWriter());

            writer->open(path_string_handle, cv::CAP_FFMPEG, fourcc, fps, *size, args);

            if (!writer->isOpened())
            {
                throw std::runtime_error("Unable to create the video file writer.");
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_VIDEOIO_EXPORT LV_MgErr_t g_ar_tk_video_io_writer_write_frame(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t writer_edvr_ref,
        LV_EDVRReferencePtr_t img_edvr_ref
    )
    {
        try
        {
            EDVRManagedObject<cv::VideoWriter> writer(writer_edvr_ref);

            if (!writer->isOpened())
            {
                throw std::runtime_error("Video writer is invalid.");
            }

            lv_image src(img_edvr_ref);

            if (src.is_greyscale())
            {
                writer->write(src);
            }
            else
            {
                cv::Mat tmp(src.size(), CV_8UC3);
                cv::cvtColor(src, tmp, cv::COLOR_BGRA2BGR);
                writer->write(tmp);
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}