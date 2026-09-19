#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <exception>
#include <set>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
    // stores the intermediate buffer between calls to minimize allocations
    class ImageEncoder
    {
    public:
        ImageEncoder() = delete;
        ImageEncoder(const std::string &ext, LV_BooleanPtr_t write_alpha_ptr);
        bool encode(lv_image &src, LV_StringHandle_t result);

    private:
        std::vector<uchar> buffer;
        const std::string ext;
        const bool write_alpha;
    };
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_encoder_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t extension_handle,
        LV_BooleanPtr_t write_alpha_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            if(!cv::haveImageWriter(extension_handle)){
                throw std::invalid_argument("No codec support for file-extension \"" + std::string(extension_handle) + "\".");
            }
            EDVRManagedObject<ImageEncoder>(edvr_ref_ptr, new ImageEncoder(extension_handle, write_alpha_ptr));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_encoder_encode(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t encoder_edvr_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_StringHandle_t buffer_handle)
    {

        try
        {
            auto success = EDVRManagedObject<ImageEncoder>(encoder_edvr_ref_ptr)->encode(lv_image(src_edvr_ref_ptr), buffer_handle);

            if (!success)
            {
                throw std::invalid_argument("Unable to encode the source image to buffer.");
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }

        return LV_ERR_noError;
    }
}

ImageEncoder::ImageEncoder(const std::string &ext, LV_BooleanPtr_t write_alpha_ptr) : ext(ext), write_alpha(*write_alpha_ptr)
{
    // nothing else to construct
}

bool ImageEncoder::encode(lv_image &src, LV_StringHandle_t result)
{
    bool success;

    if (src.is_bgra() && write_alpha || src.is_greyscale())
    {
        // colour and write ARGB or greyscale
        success = cv::imencode(ext, src, buffer);
    }
    else
    {

        cv::Mat bgr(src.size(), CV_8UC3);
        cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR);

        success = cv::imencode(ext, bgr, buffer);
    }

    if (success)
    {
        result.copy_memory_from(buffer);
        return success;
    }

    // failed - set result size to 0
    result.size_to_fit(0);
    return false;
}