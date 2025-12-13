#include <opencv2/imgproc.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{

#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    class LV_EnumSobelSize_t : public LV_EnumCVInt_t
    {
    public:
        operator int() const
        {
            // 0->3
            // 1->5
            // 2->7
            // otherwise throw
            if (m_value > 2)
            {
                throw std::invalid_argument("Sobel enumeration value should be 3 or below.");
            }
            return (m_value * 2) + 3;
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_image_canny(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t edge_map_edvr_ref_ptr,
        double threshold_1,
        double threshold_2,
        LV_EnumSobelSize_t aperture_size)
    {
        try
        {
            lv_image src(src_edvr_ref_ptr);
            lv_image edge_map(edge_map_edvr_ref_ptr);

            cv::Canny(src, edge_map, threshold_1, threshold_2, aperture_size);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}