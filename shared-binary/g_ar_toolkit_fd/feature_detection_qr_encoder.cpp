#include <vector>

#include <opencv2/objdetect.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_str.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_2d.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_QRCorrectionLevel_t : public LV_EnumCVInt_t
    {
    public:
        operator cv::QRCodeEncoder::CorrectionLevel() const
        {
            const cv::QRCodeEncoder::CorrectionLevel levels[] =
                {
                    cv::QRCodeEncoder::CORRECT_LEVEL_L,
                    cv::QRCodeEncoder::CORRECT_LEVEL_M,
                    cv::QRCodeEncoder::CORRECT_LEVEL_Q,
                    cv::QRCodeEncoder::CORRECT_LEVEL_H};

            if (m_value < std::size(levels))
            {
                return levels[m_value];
            }

            throw std::out_of_range("The supplied value for the correction level does not map to an OpenCV type.");
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    // encoder is managed through a opencv smart pointer so create a small object to hold it

    class qr_encoder_t
    {
    public:
        qr_encoder_t() = delete;
        qr_encoder_t(const cv::QRCodeEncoder::Params &params) : m_encoder(cv::QRCodeEncoder::create(params)) {}
        void encode(const std::string &encoded_info, cv::OutputArray qrcode)
        {
            m_encoder->encode(encoded_info, qrcode);
        }
        void encodeStructuredAppend(const std::string &encoded_info, std::vector<cv::Mat> &qrcodes)
        {
            m_encoder->encodeStructuredAppend(encoded_info, qrcodes);
        }

    private:
        const cv::Ptr<cv::QRCodeEncoder> m_encoder;
    };

}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_qr_encoder_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        uint8_t n_qrcodes,
        LV_QRCorrectionLevel_t correction_level)
    {
        try
        {
            cv::QRCodeEncoder::Params p;
            p.correction_level = correction_level;

            if (n_qrcodes > 0)
            {
                p.structure_number = n_qrcodes;
            }

            EDVRManagedObject<qr_encoder_t> qr_encoder(edvr_ref_ptr, new qr_encoder_t(p));
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_qr_encoder_generate(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_StringHandle_t info,
        LV_2DArrayHandle_t<uint8_t> qr_code_handle)
    {
        try
        {
            EDVRManagedObject<qr_encoder_t> qr_encoder(edvr_ref_ptr);

            cv::Mat qr_output;
            qr_encoder->encode(info, qr_output);

            qr_code_handle.copy_memory_from(qr_output);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_qr_encoder_generate_structured_append(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_StringHandle_t info,
        LV_1DArrayHandle_t<LV_2DArrayHandle_t<uint8_t>> qr_codes_handle)
    {
        try
        {
            EDVRManagedObject<qr_encoder_t> qr_encoder(edvr_ref_ptr);

            std::vector<cv::Mat> qr_outputs;
            qr_encoder->encodeStructuredAppend(info, qr_outputs);

            qr_codes_handle.copy_element_by_element_from(qr_outputs, [](auto from, auto to)
                                                         { to->copy_memory_from(from); });
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}