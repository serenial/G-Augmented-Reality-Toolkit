#include <opencv2/text.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

#include "g_ar_toolkit_tesseract_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    struct LV_TesseractResult_t
    {
        LV_StringHandle_t m_text;
        float m_confidence;
        LV_ImagePointInt_t m_rect_top_left, m_rect_bottom_right;
    };

    class LV_TesseractResultsHandle_t : private LV_1DArrayHandle_t<LV_TesseractResult_t>
    {
    public:
        LV_TesseractResultsHandle_t() = delete;
        void copy_from(const std::vector<cv::Rect> &rects, const std::vector<std::string> &texts, const std::vector<float> &confidences)
        {
            if (rects.size() != texts.size() || rects.size() != confidences.size())
            {
                throw std::out_of_range("Tesseract result vectors are not the same size.");
            }

            size_to_fit(rects.size());
            auto current_element = begin();
            for (int i = 0; i < rects.size(); i++)
            {
                current_element->m_confidence = confidences[i];
                current_element->m_text.copy_from(texts[i]);
                current_element->m_rect_bottom_right = rects[i].br();
                current_element->m_rect_top_left = rects[i].tl();
                current_element++;
            }
        }
    };

    class LV_TesseractEngineMode : public LV_EnumCVInt_t
    {
    public:
        operator cv::text::ocr_engine_mode() const
        {
            const cv::text::ocr_engine_mode modes[] =
                {
                    cv::text::ocr_engine_mode::OEM_TESSERACT_ONLY,
                    cv::text::ocr_engine_mode::OEM_CUBE_ONLY,
                    cv::text::ocr_engine_mode::OEM_TESSERACT_CUBE_COMBINED,
                    cv::text::ocr_engine_mode::OEM_DEFAULT};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the OCR mode does not map to an OpenCV type.");
        }
    };

    class LV_PageSegmentationMode : public LV_EnumCVInt_t
    {
    public:
        operator cv::text::page_seg_mode() const
        {
            const cv::text::page_seg_mode modes[] =
                {
                    cv::text::page_seg_mode::PSM_OSD_ONLY,
                    cv::text::page_seg_mode::PSM_AUTO_OSD,
                    cv::text::page_seg_mode::PSM_AUTO_ONLY,
                    cv::text::page_seg_mode::PSM_AUTO,
                    cv::text::page_seg_mode::PSM_SINGLE_COLUMN,
                    cv::text::page_seg_mode::PSM_SINGLE_BLOCK_VERT_TEXT,
                    cv::text::page_seg_mode::PSM_SINGLE_BLOCK,
                    cv::text::page_seg_mode::PSM_SINGLE_LINE,
                    cv::text::page_seg_mode::PSM_SINGLE_WORD,
                    cv::text::page_seg_mode::PSM_CIRCLE_WORD,
                    cv::text::page_seg_mode::PSM_SINGLE_CHAR};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("The supplied value for the page segmentation mode does not map to an OpenCV type.");
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    class tesseract_t
    {
    public:
        tesseract_t() = delete;
        tesseract_t(const char *datapath, const char *language, const char *char_whitelist, cv::text::ocr_engine_mode oem, cv::text::page_seg_mode psm) {
            try{
                m_ocr = cv::text::OCRTesseract::create(datapath, language, char_whitelist, oem, psm);
            }
            catch (int e)
            {
                switch (e)
                {
                default:
                    throw std::runtime_error("Could not initialize Tesseract. Ensure it is installed and the system environment variables are correctly set.");
                }
            }
            catch(...){
                throw std::current_exception();
            }

        }
        cv::Ptr<cv::text::OCRTesseract> operator->() const
        {
            return m_ocr;
        }

    private:
        cv::Ptr<cv::text::OCRTesseract> m_ocr;
    };

}

extern "C"
{
    G_AR_TOOLKIT_TESSERACT_EXPORT LV_MgErr_t g_ar_tk_fd_tesseract_text_create(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr,
        LV_StringHandle_t data_path_handle,
        LV_StringHandle_t lang_handle,
        LV_StringHandle_t whitelist_handle,
        LV_TesseractEngineMode oem,
        LV_PageSegmentationMode psm)
    {
        try
        {
            auto t = new tesseract_t(
                data_path_handle.empty() ? nullptr : static_cast<std::string>(data_path_handle).c_str(),
                lang_handle.empty() ? nullptr : static_cast<std::string>(lang_handle).c_str(),
                whitelist_handle.empty() ? nullptr : static_cast<std::string>(whitelist_handle).c_str(),
                oem, psm);

            EDVRManagedObject<tesseract_t>(edvr_ref_ptr, t);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_TESSERACT_EXPORT LV_MgErr_t g_ar_tk_fd_tesseract_text_detect(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_EDVRReferencePtr_t mask_edvr_ref_ptr,
        LV_BooleanPtr_t has_mask,
        LV_BooleanPtr_t text_line,
        LV_StringHandle_t text_result_handle,
        LV_TesseractResultsHandle_t results_handle)
    {
        try
        {
            EDVRManagedObject<tesseract_t> tesseract(detector_ref_ptr);

            std::string text_result;
            std::vector<cv::Rect> rects;
            std::vector<std::string> strings;
            std::vector<float> confidences;

            auto level = *text_line ? cv::text::OCR_LEVEL_TEXTLINE : cv::text::OCR_LEVEL_WORD;

            lv_image src(src_edvr_ref_ptr);
            cv::Mat to_ocr = src;

            if(src.is_bgra()){
                cv::cvtColor(src, to_ocr, cv::COLOR_BGRA2BGR);
            }

            if (*has_mask)
            {
                (*tesseract)->run(to_ocr, lv_image(mask_edvr_ref_ptr), text_result, &rects, &strings, &confidences, level);
            }
            else
            {
                (*tesseract)->run(to_ocr, text_result, &rects, &strings, &confidences, level);
            }

            text_result_handle.copy_from(text_result);
            results_handle.copy_from(rects, strings, confidences);
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}