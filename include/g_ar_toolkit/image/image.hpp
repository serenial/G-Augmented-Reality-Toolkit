#pragma once

#include <exception>
#include <vector>
#include <shared_mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "g_ar_toolkit/lv-interop/lv-types.hpp"


namespace g_ar_toolkit{
namespace image
{
    // create a structure to contain a cv::Mat and a locking mechanism
    struct Image
    {
        mutable std::shared_mutex mutex;
        cv::Mat mat;

        lv_interop::LV_MgErr_t set_edvr_sub_array_to_mat_data(lv_interop::LV_EDVRDataPtr_t);
    };

    lv_interop::LV_MgErr_t on_image_edvr_lock(lv_interop::LV_EDVRDataPtr_t );
    lv_interop::LV_MgErr_t on_image_edvr_unlock(lv_interop::LV_EDVRDataPtr_t);
    void on_edvr_delete(lv_interop::LV_EDVRDataPtr_t);


#include "g_ar_toolkit/lv-interop/set-packing.hpp"
    using ImageSize = struct
    {
        uint16_t width,height;
    };
#include "g_ar_toolkit/lv-interop/reset-packing.hpp"
}
}
