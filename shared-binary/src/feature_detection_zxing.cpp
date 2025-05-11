#include <vector>
#include <algorithm>
#include <bitset>
#include <type_traits>

#include <opencv2/objdetect.hpp>
#include <zxing/ReadBarcode.h>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{

    const ZXing::BarcodeFormat barcode_formats[] = {
        ZXing::BarcodeFormat::Aztec,
        ZXing::BarcodeFormat::Codabar,
        ZXing::BarcodeFormat::Code39,
        ZXing::BarcodeFormat::Code93,
        ZXing::BarcodeFormat::Code128,
        ZXing::BarcodeFormat::DataBar,
        ZXing::BarcodeFormat::DataBarExpanded,
        ZXing::BarcodeFormat::DataMatrix,
        ZXing::BarcodeFormat::EAN8,
        ZXing::BarcodeFormat::EAN13,
        ZXing::BarcodeFormat::ITF,
        ZXing::BarcodeFormat::MaxiCode,
        ZXing::BarcodeFormat::PDF417,
        ZXing::BarcodeFormat::QRCode,
        ZXing::BarcodeFormat::UPCA,
        ZXing::BarcodeFormat::UPCE,
        ZXing::BarcodeFormat::MicroQRCode,
        ZXing::BarcodeFormat::RMQRCode,
        ZXing::BarcodeFormat::DXFilmEdge,
        ZXing::BarcodeFormat::DataBarLimited};

#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_EnumBarcodeFormat_t
    {
        uint8_t m_value;

    public:
        LV_EnumBarcodeFormat_t() = delete;
        LV_EnumBarcodeFormat_t &operator=(ZXing::BarcodeFormat f)
        {

            auto found = std::find(std::begin(barcode_formats), std::end(barcode_formats), f);

            if (found == std::end(barcode_formats))
            {
                throw std::invalid_argument("Barcode Format value is out of range.");
            }

            m_value = std::distance(std::begin(barcode_formats), found);

            return *this;
        }
        static ZXing::BarcodeFormat flatten_to_zxing_barcode_format(const LV_1DArrayHandle_t<LV_EnumBarcodeFormat_t> handle)
        {
            return std::accumulate(handle.begin(), handle.end(), ZXing::BarcodeFormat::None, [](ZXing::BarcodeFormat f, LV_EnumBarcodeFormat_t v)
                                   {
                                      

                                       if (v.m_value < std::size(barcode_formats))
                                       {
                                           auto result = static_cast<std::underlying_type_t<ZXing::BarcodeFormat>>(f) | static_cast<std::underlying_type_t<ZXing::BarcodeFormat>>(barcode_formats[v.m_value]);
                                           return static_cast<ZXing::BarcodeFormat>(result);
                                       }

                                       throw std::out_of_range("Barcode Format value is out of range."); });
        }
    };

    class LV_EnumBinarizer_t
    {
        uint8_t m_value;

    public:
        LV_EnumBinarizer_t() = delete;
        operator ZXing::Binarizer() const
        {
            const ZXing::Binarizer modes[] = {
                ZXing::Binarizer::LocalAverage,
                ZXing::Binarizer::GlobalHistogram,
                ZXing::Binarizer::FixedThreshold,
                ZXing::Binarizer::BoolCast};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("Binarizer Value is out of range");
        }
    };

    class LV_EanAddOnSymbol_t
    {
        uint8_t m_value;

    public:
        LV_EanAddOnSymbol_t() = delete;
        operator ZXing::EanAddOnSymbol() const
        {
            const ZXing::EanAddOnSymbol modes[] =
                {
                    ZXing::EanAddOnSymbol::Ignore,
                    ZXing::EanAddOnSymbol::Read,
                    ZXing::EanAddOnSymbol::Require};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("EanAddOnSymbol Value is out of range");
        }
    };

    class LV_EnumTextMode_t
    {
        uint8_t m_value;

    public:
        LV_EnumTextMode_t() = delete;
        operator ZXing::TextMode() const
        {
            const ZXing::TextMode modes[] =
                {
                    ZXing::TextMode::Plain,
                    ZXing::TextMode::ECI,
                    ZXing::TextMode::HRI,
                    ZXing::TextMode::Hex,
                    ZXing::TextMode::Escaped};

            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("Text Mode Value is out of range");
        }
    };

    class LV_EnumCharacterSet_t
    {
        uint8_t m_value;

    public:
        LV_EnumCharacterSet_t() = delete;
        operator ZXing::CharacterSet() const
        {
            const ZXing::CharacterSet modes[] = {
                ZXing::CharacterSet::ASCII,
                ZXing::CharacterSet::ISO8859_1,
                ZXing::CharacterSet::ISO8859_2,
                ZXing::CharacterSet::ISO8859_3,
                ZXing::CharacterSet::ISO8859_4,
                ZXing::CharacterSet::ISO8859_5,
                ZXing::CharacterSet::ISO8859_6,
                ZXing::CharacterSet::ISO8859_7,
                ZXing::CharacterSet::ISO8859_8,
                ZXing::CharacterSet::ISO8859_9,
                ZXing::CharacterSet::ISO8859_10,
                ZXing::CharacterSet::ISO8859_11,
                ZXing::CharacterSet::ISO8859_13,
                ZXing::CharacterSet::ISO8859_14,
                ZXing::CharacterSet::ISO8859_15,
                ZXing::CharacterSet::ISO8859_16,
                ZXing::CharacterSet::Cp437,
                ZXing::CharacterSet::Cp1250,
                ZXing::CharacterSet::Cp1251,
                ZXing::CharacterSet::Cp1252,
                ZXing::CharacterSet::Cp1256,
                ZXing::CharacterSet::Shift_JIS,
                ZXing::CharacterSet::Big5,
                ZXing::CharacterSet::GB2312,
                ZXing::CharacterSet::GB18030,
                ZXing::CharacterSet::EUC_JP,
                ZXing::CharacterSet::EUC_KR,
                ZXing::CharacterSet::UTF16BE,
                ZXing::CharacterSet::UTF8,
                ZXing::CharacterSet::UTF16LE,
                ZXing::CharacterSet::UTF32BE,
                ZXing::CharacterSet::UTF32LE,
                ZXing::CharacterSet::BINARY};
            if (m_value < std::size(modes))
            {
                return modes[m_value];
            }

            throw std::out_of_range("Character Set is out of range");
        }
    };

    class LV_ZXingOptions_t
    {
        LV_Boolean_t try_harder, try_rotate, try_invert, try_downscale, is_pure, try_code39_extended_mode, return_errors;
        uint8_t downscale_factor, min_line_count, max_number_of_symbols;
        LV_EnumBinarizer_t binarizer;
        LV_EanAddOnSymbol_t ean_add_on_symbol;
        LV_EnumTextMode_t text_mode;
        LV_EnumCharacterSet_t character_set;
        uint16_t downscale_threshold;
        LV_1DArrayHandle_t<LV_EnumBarcodeFormat_t> formats_handle;
        uint16_t non_default;

    public:
        operator ZXing::ReaderOptions() const
        {
            ZXing::ReaderOptions opts;
            std::bitset<16> is_set(non_default);

            if (is_set.test(0))
            {
                opts.setTryHarder(try_harder);
            }

            if (is_set.test(1))
            {
                opts.setTryRotate(try_rotate);
            }

            if (is_set.test(2))
            {
                opts.setTryInvert(try_invert);
            }

            if (is_set.test(3))
            {
                opts.setTryDownscale(try_downscale);
            }

            if (is_set.test(4))
            {
                opts.setIsPure(is_pure);
            }

            if (is_set.test(5))
            {
                opts.setTryHarder(try_code39_extended_mode);
            }

            if (is_set.test(6))
            {
                opts.setReturnErrors(return_errors);
            }

            if (is_set.test(7))
            {
                opts.setDownscaleFactor(downscale_factor);
            }

            if (is_set.test(8))
            {
                opts.setMinLineCount(min_line_count);
            }

            if (is_set.test(9))
            {
                opts.setMaxNumberOfSymbols(max_number_of_symbols);
            }

            if (is_set.test(10)){
                opts.setBinarizer(binarizer);
            }
            
            if (is_set.test(11)){
                opts.setEanAddOnSymbol(ean_add_on_symbol);
            }
            
            if (is_set.test(12)){
                opts.setTextMode(text_mode);
            }

            if (is_set.test(13)){
                opts.setCharacterSet(character_set);
            }

            if (is_set.test(14)){
                opts.setDownscaleThreshold(downscale_threshold);
            }

            opts.setFormats(formats_handle.empty()? ZXing::BarcodeFormat::Any : LV_EnumBarcodeFormat_t::flatten_to_zxing_barcode_format(formats_handle));

            return opts;
        }
    };

    struct LV_ZXingBarcode_t
    {
        LV_ImagePointInt_t m_a, m_b, m_c, m_d;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_zxing_read_barcodes(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_Ptr_t<LV_ZXingOptions_t> options_ptr)
    {
        try
        {
            lv_image image(src_edvr_ref_ptr);

            auto barcodes = ZXing::ReadBarcodes(image, *options_ptr);

            for (const auto &barcode : barcodes)
            {
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}