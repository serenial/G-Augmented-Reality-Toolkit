#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/lv-interop/lv-array.hpp"
#include "g_ar_toolkit/lv-interop/lv-image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

#include "g_ar_toolkit/lv-interop/set-packing.hpp"
using LV_RectangleCorners_t = struct
{
    int16_t left, top, right, bottom;
};

using LV_PixmapImage_t = struct
{
    int32_t image_type;
    int32_t image_depth;
    LV_Handle_t<LV_Array_t<1, uint8_t>> image_array_handle;
    LV_Handle_t<LV_Array_t<1, uint8_t>> mask_array_handle;
    LV_Handle_t<LV_Array_t<1, uint32_t>> colour_array_handle;
    LV_RectangleCorners_t rect;
};

using LV_PixmapImagePtr_t = LV_Ptr_t<LV_PixmapImage_t>;

#include "g_ar_toolkit/lv-interop/reset-packing.hpp"

void copy_lv_mask_to_cv_mat(LV_PixmapImagePtr_t, size_t, cv::Mat &);

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy_from_lv_pixmap(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_PixmapImagePtr_t pixmap_image_ptr,
        LV_EDVRReferencePtr_t dst_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask_ptr)
    {
        try
        {
            bool has_mask = *has_mask_ptr;

            throw_if_edvr_ref_pointers_not_unique({dst_edvr_ref_ptr, mask_edvr_ref_ptr});

            lv_image dst(dst_edvr_ref_ptr);

            cv::Mat working_mat;
            cv::Mat *working_mat_ptr;

            // create rectangle
            cv::Rect2i rectangle(cv::Point2i{pixmap_image_ptr->rect.left, pixmap_image_ptr->rect.top}, cv::Point2i{pixmap_image_ptr->rect.right, pixmap_image_ptr->rect.bottom});

            // determine number of bytes in the pixmap mask
            size_t number_of_mask_bytes = (*pixmap_image_ptr->mask_array_handle)->dims ? (*pixmap_image_ptr->mask_array_handle)->dims[0] : 0;

            bool flatten_mask_to_dst = !has_mask && number_of_mask_bytes > 0;

            if (flatten_mask_to_dst)
            {
                if (dst.size() != rectangle.size())
                {
                    throw std::invalid_argument("The pixmap contains mask data but the destination image is not correctly initialized. " 
                                                " Either use <b>Copy Pixmap to Image with Mask Output.vi</b> to copy the mask information"
                                                " or intialize the destination image pixels to specify the background image.");
                }

                working_mat = cv::Mat(rectangle.size(), dst.cv_type());
                working_mat_ptr = &working_mat;
            }
            else
            {
                dst.ensure_sized_to_match(rectangle);
                working_mat_ptr = &(*dst);
            }

            auto data_ptr = (*pixmap_image_ptr->image_array_handle)->data_ptr();

            switch (pixmap_image_ptr->image_depth)
            {
            case 8:
            {
                cv::Mat pixmap_image_data_mat(rectangle.size(), CV_8UC1, data_ptr, rectangle.width + rectangle.width % 2);
                size_t size_of_colour_map = (*pixmap_image_ptr->colour_array_handle)->dims ? (*pixmap_image_ptr->colour_array_handle)->dims[0] : 0;

                if (size_of_colour_map > 0)
                {
                    // using colour map
                    // create Mat around colourMap Array
                    cv::Mat lookup_table(size_of_colour_map, 1, CV_8UC4, (*pixmap_image_ptr->colour_array_handle)->data_ptr());

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
                        for (size_t row = 0; row < dst.height(); row++)
                        {
                            for (size_t column = 0; column < dst.width(); column++)
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
                cv::Mat mask(dst.size(), CV_8UC1);
                copy_lv_mask_to_cv_mat(pixmap_image_ptr, number_of_mask_bytes, mask);
                working_mat_ptr->copyTo((*dst), mask);
                return LV_ERR_noError;
            }

            lv_image mask(mask_edvr_ref_ptr);

            mask.ensure_sized_to_match(rectangle);

            // lv-mask is empty
            if (number_of_mask_bytes == 0)
            {
                // mask requested but no mask data set create all 255 mask
                (*mask) = cv::Scalar(255);
                // all done
                return LV_ERR_noError;
            }

            // copy lv mask into mask-image
            copy_lv_mask_to_cv_mat(pixmap_image_ptr, number_of_mask_bytes, *mask);
        }
        catch (...)
        {
            return caught_exception_to_lv_err(std::current_exception(), error_cluster_ptr, __func__);
        }

        return LV_ERR_noError;
    }
}

void copy_lv_mask_to_cv_mat(LV_PixmapImagePtr_t pixmap_image_ptr, size_t number_of_mask_bytes, cv::Mat &mask)
{
    auto mask_bytes_ptr = (*pixmap_image_ptr->mask_array_handle)->data_ptr();
    auto mask_bytes_end = mask_bytes_ptr + number_of_mask_bytes;

    for (size_t row = 0; row < mask.rows && mask_bytes_ptr < mask_bytes_end; row++)
    {
        const uint8_t bitmask = 0b1000'0000;
        size_t col = 0;

        while (col < mask.cols && mask_bytes_ptr < mask_bytes_end)
        {
            // bitwise-AND with bitmask to check single bit value
            mask.at<uint8_t>(row, col) = (*mask_bytes_ptr & (bitmask >> (col % 8)))? 255 : 0;
            // increment col and shift bitmask
            col++;
            if (col % 8 == 0)
            {
                mask_bytes_ptr++;
            }
        }
        // account for end of row padding
        int32_t excess_bits = (16 - mask.cols % 16) % 16;
        while (excess_bits > 0)
        {
            mask_bytes_ptr++;
            excess_bits -= 8;
        }
    }
}
