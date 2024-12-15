#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_read_file(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t path_string_handle,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        uint8_t downsize_enum)
    {
        try
        {
            int flags = 0;
            lv_image dst(dst_edvr_ref_ptr);

            if (dst.is_greyscale())
            {
                switch (downsize_enum)
                {
                case 1:
                    flags += cv::IMREAD_REDUCED_GRAYSCALE_2;
                    break;
                case 2:
                    flags += cv::IMREAD_REDUCED_GRAYSCALE_4;
                    break;
                case 3:
                    flags += cv::IMREAD_REDUCED_GRAYSCALE_8;
                    break;

                default:
                    flags += cv::IMREAD_GRAYSCALE;
                }
            }
            else
            {
                switch (downsize_enum)
                {
                case 1:
                    flags += cv::IMREAD_REDUCED_COLOR_2;
                    break;
                case 2:
                    flags += cv::IMREAD_REDUCED_COLOR_4;
                    break;
                case 3:
                    flags += cv::IMREAD_REDUCED_COLOR_8;
                    break;
                default:
                    flags += cv::IMREAD_ANYCOLOR;
                }
            }

            auto input = cv::imread(path_string_handle, flags);

            if (dst.is_greyscale())
            {
                dst.set_mat(input);
                return LV_ERR_noError;
            }

            switch (input.channels())
            {
            case 1:
            {
                // create alpha channel as max unsigned-8-bit;
                cv::Mat alpha(input.size(), CV_8UC1, cv::Scalar(255));
                // greyscale into dst and add alpha channel (BGRA)
                if (dst.size() != input.size())
                {
                    dst.set_mat(cv::Mat(input.size(), CV_8UC4));
                }
                cv::mixChannels(std::begin({input, alpha}), 2, static_cast<cv::Mat *>(dst), 1, std::begin({0, 0, 0, 1, 0, 2, 1, 3}), 4);
            }
            break;
            case 3:
                cv::cvtColor(input, dst, cv::COLOR_BGR2BGRA);
                break;
            case 4:
                dst.set_mat(input);
                break;
            default:
                throw std::out_of_range("Only greyscale and 3 or 4 colour input images are supported.");
            }
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}