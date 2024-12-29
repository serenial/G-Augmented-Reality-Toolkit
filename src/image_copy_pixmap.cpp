#include <memory>
#include <numeric>
#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_u32_colour.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    using LV_RectangleCorners_t = struct
    {
        int16_t left, top, right, bottom;
    };

    class LV_PixmapMaskArrayHandle_t : public LV_1DArrayHandle_t<uint8_t>
    {
    public:
        void copy_to_cv_mat_type_mask(cv::Mat &mask) const;
        void copy_from_cv_mat_type_mask(const cv::Mat &mask);

    private:
        std::pair<int, size_t> calc_mask_buffer_bytes_and_stride(const cv::Mat &) const;
    };

    using LV_PixmapImage_t = struct
    {
        int32_t image_type;
        int32_t image_depth;
        LV_1DArrayHandle_t<uint8_t> image_array_handle;
        LV_PixmapMaskArrayHandle_t mask_array_handle;
        LV_1DArrayHandle_t<LV_U32RGBColour_t> colour_array_handle;
        LV_RectangleCorners_t rect;
    };

    using LV_PixmapImagePtr_t = LV_Ptr_t<LV_PixmapImage_t>;

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_pixmap(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PixmapImagePtr_t pixmap_image_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr,
        LV_U32RGBColour_t background_value)
    {
        try
        {
            bool has_mask = *has_mask_ptr;

            if (has_mask)
            {
                throw_if_edvr_ref_pointers_not_unique({dst_edvr_ref_ptr, mask_edvr_ref_ptr});
            }

            lv_image dst(dst_edvr_ref_ptr);

            cv::Mat working_mat;

            cv::Size required_dst_size(cv::Point2i{pixmap_image_ptr->rect.right, pixmap_image_ptr->rect.bottom});

            // create rectangle
            cv::Rect2i rectangle(cv::Point2i{pixmap_image_ptr->rect.left, pixmap_image_ptr->rect.top}, cv::Point2i{pixmap_image_ptr->rect.right, pixmap_image_ptr->rect.bottom});

            // determine number of bytes in the pixmap mask
            size_t number_of_mask_bytes = pixmap_image_ptr->mask_array_handle.size();

            bool flatten_mask_to_dst = !has_mask && number_of_mask_bytes > 0;

            if (flatten_mask_to_dst)
            {
                if (dst.size() != required_dst_size)
                {
                    throw std::invalid_argument("The pixmap contains mask data but the destination image is not correctly initialized. "
                                                " Either use <b>Copy Pixmap to Image with Mask Output.vi</b> to copy the mask information"
                                                " or intialize the destination image pixels to specify the background image.");
                }

                // just create a working mat that is the required rectangle size
                working_mat = cv::Mat(rectangle.size(), dst.cv_type());
            }
            else
            {
                dst.ensure_sized_to_match(required_dst_size);

                if (required_dst_size != rectangle.size())
                {
                    // fill with specified background colour
                    if (dst.is_bgra())
                    {
                        (*dst) = background_value.get_bgra();
                    }
                    else
                    {
                        (*dst) = background_value.get_blue();
                    }
                }
                working_mat = dst(rectangle);
            }

            cv::Mat *working_mat_ptr = &(working_mat);

            auto data_ptr = pixmap_image_ptr->image_array_handle.begin();

            switch (pixmap_image_ptr->image_depth)
            {
            case 8:
            {
                cv::Mat pixmap_image_data_mat(rectangle.size(), CV_8UC1, data_ptr, rectangle.width + rectangle.width % 2);
                size_t size_of_colour_map = pixmap_image_ptr->colour_array_handle.size();

                if (size_of_colour_map > 0)
                {
                    // using colour map
                    // create Mat around colourMap Array
                    cv::Mat lookup_table(size_of_colour_map, 1, CV_8UC4, pixmap_image_ptr->colour_array_handle.begin());

                    if (dst.is_greyscale())
                    {
                        // create a greyscale lookup table first and then apply
                        cv::Mat greyscale_lookup_table(lookup_table.size(), CV_8UC1);
                        cv::cvtColor(lookup_table, greyscale_lookup_table, cv::COLOR_BGRA2GRAY);
                        cv::LUT(pixmap_image_data_mat, greyscale_lookup_table, *working_mat_ptr);
                    }
                    else
                    {
                        // apply lookup pixel-by-pixel as we are reading 4 channel lookup on a single channel source
                        for (size_t row = 0; row < rectangle.height; row++)
                        {
                            for (size_t column = 0; column < rectangle.width; column++)
                            {
                                auto index = pixmap_image_data_mat.at<uint8_t>(row, column);
                                working_mat_ptr->at<cv::Vec4b>(row, column) = lookup_table.at<cv::Vec4b>(index);
                                ;
                            }
                        }
                    }
                }
                else
                {
                    // no colour map
                    if (dst.is_greyscale())
                    {
                        // use single channel data directly
                        *working_mat_ptr = pixmap_image_data_mat;
                    }
                    else
                    {
                        // convert greyscale to BGRA
                        cv::cvtColor(pixmap_image_data_mat, *working_mat_ptr, cv::COLOR_GRAY2BGRA);
                    }
                }
                break;
            }
            case 24:
            {
                cv::Mat pixmap_image_data_mat(rectangle.size(), CV_8UC3, data_ptr);

                if (dst.is_greyscale())
                {
                    cv::cvtColor(pixmap_image_data_mat, *working_mat_ptr, cv::COLOR_RGB2GRAY);
                }
                else
                {
                    cv::cvtColor(pixmap_image_data_mat, *working_mat_ptr, cv::COLOR_RGB2BGRA);
                }
                break;
            }
            case 32:
            {
                cv::Mat pixmap_image_data_mat(rectangle.size(), CV_8UC4, data_ptr);
                if (dst.is_greyscale())
                {
                    cv::cvtColor(pixmap_image_data_mat, *working_mat_ptr, cv::COLOR_RGBA2GRAY);
                }
                else
                {
                    // rearrange channels (ARGB to BGRA)
                    cv::cvtColor(pixmap_image_data_mat, *working_mat_ptr, cv::COLOR_RGBA2BGRA);
                }
                break;
            }
            default:
                throw std::out_of_range("Unsupported Pixmap image Type. Use 8,24 or 32 bit image.");
            }

            if (!has_mask)
            {
                if (number_of_mask_bytes == 0)
                {
                    // no mask data - all done
                    return LV_ERR_noError;
                }

                // flatten the mask into the dst
                cv::Mat mask(rectangle.size(), CV_8UC1);
                pixmap_image_ptr->mask_array_handle.copy_to_cv_mat_type_mask(mask);
                working_mat_ptr->copyTo(dst(rectangle), mask);
                return LV_ERR_noError;
            }

            lv_image mask(mask_edvr_ref_ptr);

            mask.ensure_sized_to_match(required_dst_size);

            // lv-mask is empty
            if (number_of_mask_bytes == 0)
            {
                // mask requested but no mask data set create all 255 mask
                (*mask) = cv::Scalar(255);
                // all done
                return LV_ERR_noError;
            }

            // copy lv mask into mask-image
            cv::Mat mask_roi = mask(rectangle);
            pixmap_image_ptr->mask_array_handle.copy_to_cv_mat_type_mask(mask_roi);
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_to_lv_pixmap(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr,
        LV_ImagePointIntPtr_t offset_ptr,
        LV_PixmapImagePtr_t pixmap_image_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);

            // set other pixmap fields
            pixmap_image_ptr->image_type = 0;
            pixmap_image_ptr->image_depth = src.is_bgra() ? 24 : 8;
            pixmap_image_ptr->rect.left = offset_ptr->m_x;
            pixmap_image_ptr->rect.top = offset_ptr->m_y;
            pixmap_image_ptr->rect.right = pixmap_image_ptr->rect.left + src.width();
            pixmap_image_ptr->rect.bottom = pixmap_image_ptr->rect.top + src.height();

            if (*has_mask_ptr)
            {
                lv_image mask(mask_edvr_ref_ptr);

                pixmap_image_ptr->mask_array_handle.copy_from_cv_mat_type_mask(mask);
            }
            else
            {
                pixmap_image_ptr->mask_array_handle.dispose();
            }

            if (src.is_greyscale())
            {

                auto bytes_per_row = src.width() + src.width() % 2;

                pixmap_image_ptr->image_array_handle.size_to_fit(bytes_per_row * src.height());

                // wrap image_array into a cv::Mat for easy copying into
                cv::Mat image_buffer(src.size(), CV_8UC1, pixmap_image_ptr->image_array_handle.begin(), bytes_per_row);

                src.copyTo(image_buffer);
                // set colour array to be numbers 0 to 255
                pixmap_image_ptr->colour_array_handle.size_to_fit(256);
                std::iota(pixmap_image_ptr->colour_array_handle.begin(), pixmap_image_ptr->colour_array_handle.end(), LV_U32RGBColour_t{});
                return LV_ERR_noError;
            }

            // not greyscale
            pixmap_image_ptr->image_array_handle.size_to_fit(src.size().area() * 3);
            cv::Mat image_buffer(src.size(), CV_8UC3, pixmap_image_ptr->image_array_handle.begin());
            cv::cvtColor(src, image_buffer, cv::COLOR_BGRA2RGB);
            pixmap_image_ptr->colour_array_handle.dispose();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}

void LV_PixmapMaskArrayHandle_t::copy_from_cv_mat_type_mask(const cv::Mat &mask)
{
    int number_of_mask_bytes_per_row;
    size_t stride;
    std::tie(number_of_mask_bytes_per_row, stride) = calc_mask_buffer_bytes_and_stride(mask);

    size_to_fit(stride * mask.rows);

    // set all values to 255
    std::fill(begin(), end(), 255);

    // wrap mask buffer into cv::Mat to help with handling stride
    cv::Mat mask_buffer(mask.rows, number_of_mask_bytes_per_row, CV_8UC1, begin(), stride);

    for (int row = 0; row < mask.rows; row++)
    {

        for (int col = 0; col < mask.cols; col++)
        {
            if (mask.at<uint8_t>(row, col) == 0)
            {
                // set the bit in the mask for this pixel to off (i.e masked)
                auto [mask_buffer_col, shift_by] = std::div(col, 8);
                auto bitmask = uint8_t(0b1000'0000) >> shift_by;
                // bitwise and with the bitwise negation of the bitmask
                mask_buffer.at<uint8_t>(row, mask_buffer_col) = mask_buffer.at<uint8_t>(row, mask_buffer_col) & (~bitmask);
            }
        }
    }
}

void LV_PixmapMaskArrayHandle_t::copy_to_cv_mat_type_mask(cv::Mat &mask) const
{
    int number_of_mask_bytes_per_row;
    size_t stride;
    std::tie(number_of_mask_bytes_per_row, stride) = calc_mask_buffer_bytes_and_stride(mask);

    // wrap mask buffer into cv::Mat to help with handling stride
    cv::Mat mask_buffer(mask.rows, number_of_mask_bytes_per_row, CV_8UC1, begin(), stride);

    for (int row = 0; row < mask.rows; row++)
    {
        for (int col = 0; col < mask.cols; col++)
        {
            auto [mask_buffer_col, shift_by] = std::div(col, 8);
            auto bitmask = uint8_t(0b1000'0000) >> shift_by;
            // check if the bit of mask_buffer is 0 or not
            mask.at<uint8_t>(row, col) = ((mask_buffer.at<uint8_t>(row, mask_buffer_col)) & bitmask) == 0 ? 0 : 255;
        }
    }
}

std::pair<int, size_t> LV_PixmapMaskArrayHandle_t::calc_mask_buffer_bytes_and_stride(const cv::Mat &mask) const
{
    // mask has to have an even number of bytes per row
    auto [quotient, remainder] = std::div(mask.cols, 8);
    auto number_of_mask_bytes_per_row = quotient;

    if (remainder != 0)
    {
        ++number_of_mask_bytes_per_row;
    }

    size_t stride = number_of_mask_bytes_per_row;

    if (number_of_mask_bytes_per_row % 2 != 0)
    {
        stride++;
    }
    return std::make_pair(number_of_mask_bytes_per_row, stride);
}