#include <cstdlib>

#include <opencv2/imgproc.hpp>


#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_picture.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

using LV_PictureTopLeft_t = struct
{
    int16_t left, top;
};

using LV_PictureOpHeader_t = struct
{
    int16_t op_code;
    int32_t length, image_bytes_length;
    int16_t top, left, bottom, right;
    int16_t bitwidth;
    uint32_t default_foreground, default_background;
};

using LV_PictureTopLeftPtr_t = LV_Ptr_t<LV_PictureTopLeft_t>;
using LV_PictureOpHeaderPtr_t = LV_Ptr_t<LV_PictureOpHeader_t>;

// Platform independent size of LV_PictureOpHeader_t when flattened by LV
const uint8_t SIZEOF_LV_FLATTENED_PICTURE_OP_HEADER_BYTES = 28; 

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_draw_picture(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_PictureTopLeftPtr_t top_left_ptr,
        LV_StringHandle_t lv_str_handle,
        LV_PictureOpHeaderPtr_t op_header_ptr,
        LV_BooleanPtr_t use_mask_ptr)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);
            cv::Size dst_size = src.size();
            auto use_mask = *use_mask_ptr;
            size_t mask_row_length = 0, mask_bytes_length = 0;

            if (src.is_greyscale() && src.width() % 2 != 0)
            {
                (dst_size.width)++;
            }

            size_t image_bytes_length;
            size_t total_data_length;

            if (src.is_bgra())
            {
                image_bytes_length = dst_size.area() * 3;
                total_data_length = SIZEOF_LV_FLATTENED_PICTURE_OP_HEADER_BYTES + 2 + image_bytes_length; // add sizeof uint16_t to account for unused colour table
            }
            else
            {
                image_bytes_length = dst_size.area();
                total_data_length = SIZEOF_LV_FLATTENED_PICTURE_OP_HEADER_BYTES + image_bytes_length + lv_picture::greyscale_lookup_length;
            }

            if (use_mask)
            {
                auto qr = std::div(src.width(), 16);
                mask_row_length = qr.quot*2 + (qr.rem ? 2 : 0);
                mask_bytes_length = mask_row_length * src.height();
                total_data_length += mask_bytes_length;
            }

            // size source string-handle to hold our image data
            lv_str_handle.size_to_fit(total_data_length);

            // use some magic numbers to set the header
            op_header_ptr->op_code = use_mask ? 40 : 29; // draw rgb data
            op_header_ptr->image_bytes_length = image_bytes_length;
            op_header_ptr->bitwidth = src.is_bgra() ? 24 : 8;
            op_header_ptr->default_background = 0x00FFFFFF; // white
            op_header_ptr->default_foreground = 0x00000000; // black
            op_header_ptr->top = top_left_ptr->top;
            op_header_ptr->left = top_left_ptr->left;
            op_header_ptr->bottom = top_left_ptr->top + src.height();
            op_header_ptr->right = top_left_ptr->left + src.width();

            // get a pointer to the point in the "string" bytes where the rgb pixel data starts
            // we can then wrap this in a cv::Mat to make copying data to it easier
            auto dst_data_ptr = lv_str_handle.begin() + SIZEOF_LV_FLATTENED_PICTURE_OP_HEADER_BYTES;

            if (src.is_bgra())
            {
                op_header_ptr->length = image_bytes_length + mask_bytes_length + 24; // 24 = 22 + 2 bytes for the empty mask

                // set the first two bytes to zero which is a uint16_t value representing that the colour table is not used (length 0)

                for(size_t i=0;i<2;i++){
                    *dst_data_ptr = 0;
                    dst_data_ptr++;
                }

                cv::Mat dst(dst_size, CV_8UC3, dst_data_ptr);
                // copy channels to dst using cv::mixChannels
                // drop alpha and swap bgr to rgb
                cv::mixChannels(src, 1, &dst, 1, std::begin({0, 2, 1, 1, 2, 0}), 3);
            }
            else
            {
                op_header_ptr->length = image_bytes_length + 22 + lv_picture::greyscale_lookup_length + mask_bytes_length;

                // copy the greyscale colour-table into string
                std::memcpy(dst_data_ptr, lv_picture::greyscale_lookup, lv_picture::greyscale_lookup_length);

                dst_data_ptr += lv_picture::greyscale_lookup_length;

                cv::Mat dst(dst_size, CV_8UC1, dst_data_ptr);

                cv::Mat src_sized_dst = dst(cv::Rect(cv::Point(0, 0), src.size()));
                src.copyTo(src_sized_dst);
            }
            if (use_mask)
            {
                lv_image mask(mask_edvr_ref_ptr);
                auto mask_bytes_ptr = dst_data_ptr + image_bytes_length;
                auto mask_bytes_end = mask_bytes_ptr + mask_bytes_length;

                // initialize first byte
                *mask_bytes_ptr = 255;

                for (size_t row = 0; row < mask.height() && mask_bytes_ptr < mask_bytes_end; row++)
                {
                    const uint8_t bitmask = 0b0111'1111;
                    size_t col = 0;

                    while (col < mask.width() && mask_bytes_ptr < mask_bytes_end)
                    {
                        if ((*mask).at<uint8_t>(row, col) < 255)
                        {
                            // masked-bit - set the bit to 0
                            *mask_bytes_ptr = *mask_bytes_ptr & (bitmask >> col % 8);
                        }
                        col++;

                        if (col % 8 == 0)
                        {
                            mask_bytes_ptr++;
                            *mask_bytes_ptr = 255;
                        }
                    }
                    // account for end of row padding
                    int32_t excess_bits = mask.width() % 16;
                    while (excess_bits > 0)
                    {
                        mask_bytes_ptr++;
                        *mask_bytes_ptr = 255;
                        excess_bits -= 8;
                    }
                }
            }
        }
        catch (...)
        {
           error_cluster_ptr->copy_from_exception(std::current_exception(),__func__);
        }

        return LV_ERR_noError;
    }
}