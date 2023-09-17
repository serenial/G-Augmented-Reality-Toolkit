#include "g_ar_toolkit/lv-interop/lv-functions.hpp"
#include "g_ar_toolkit/lv-interop/lv-error.hpp"
#include "g_ar_toolkit/image/image.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_copy(LV_ErrorClusterPtr_t error_cluster_ptr, LV_EDVRReferencePtr_t src_edvr_ref_ptr, LV_EDVRReferencePtr_t dst_edvr_ref_ptr, LV_EDVRReferencePtr_t mask_edvr_ref_ptr)
    {
        try
        {
            LV_EDVRContext_t ctx;
            LV_EDVRDataPtr_t src_edvr_image_ptr = nullptr;

            auto err = get_edvr_data_handle_with_context(*src_edvr_ref_ptr, &ctx, &src_edvr_image_ptr);

            if (err)
            {
                return err;
            }

            LV_EDVRDataPtr_t dst_edvr_image_ptr = nullptr;

            err = EDVR_AddRefWithContext(*dst_edvr_ref_ptr, ctx, &dst_edvr_image_ptr);

            if (err)
            {
                return err;
            }

            LV_EDVRDataPtr_t mask_edvr_image_ptr = nullptr;

            auto no_mask = EDVR_AddRefWithContext(*mask_edvr_ref_ptr, ctx, &mask_edvr_image_ptr);

            auto src_img_ptr = reinterpret_cast<image::Image *>(src_edvr_image_ptr->metadata_ptr);
            auto dst_img_ptr = reinterpret_cast<image::Image *>(dst_edvr_image_ptr->metadata_ptr);
            // lock the mutex
            dst_img_ptr->mutex.lock();

            if (no_mask)
            {
                // copy without reading mask data
                src_img_ptr->mat.copyTo(dst_img_ptr->mat);
            }
            else
            {
                // copy with mask
                auto mask_img_ptr = reinterpret_cast<image::Image *>(mask_edvr_image_ptr->metadata_ptr);
                // copy
                src_img_ptr->mat.copyTo(dst_img_ptr->mat, mask_img_ptr->mat);
                // release mask
                err = EDVR_ReleaseRefWithContext(*mask_edvr_ref_ptr, ctx);
            }

            // release the src
            auto err2 = EDVR_ReleaseRefWithContext(*src_edvr_ref_ptr, ctx);

            // set the destination edvr subArray
            dst_img_ptr->set_edvr_sub_array_to_mat_data(dst_edvr_image_ptr);

            // release mutex and dst EDVR reference count
            dst_img_ptr->mutex.unlock();
            auto err3 = EDVR_ReleaseRefWithContext(*dst_edvr_ref_ptr, ctx);

            return err ? err : (err2 ? err2 : err3);
        }
        catch (cv::Exception &e)
        {
            return write_to_lv_error_cluster_ptr(error_cluster_ptr, e.code, __FUNCTION__, e.what());
        }
        catch (std::exception &e)
        {
            return write_to_lv_error_cluster_ptr(error_cluster_ptr, LV_ERR_bogusError, __FUNCTION__, e.what());
        }
        catch (...)
        {
            return write_undefined_err_to_lv_error_cluster_ptr(error_cluster_ptr, __FUNCTION__);
        }
    }
}