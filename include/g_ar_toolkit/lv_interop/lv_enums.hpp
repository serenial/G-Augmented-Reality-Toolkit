#ifndef G_AR_TK__INTEROP_LV_ENUMS_HPP_
#define G_AR_TK__INTEROP_LV_ENUMS_HPP_

#include <opencv2/core/base.hpp>
#include <opencv2/imgproc.hpp>

#include "./lv_array_1d.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

#include "./set_packing.hpp"

        class LV_EnumCVBoarderType_t
        {
        public:
            operator cv::BorderTypes() const;

        private:
            uint8_t m_value;
        };

        class LV_EnumCVInterpolationFlag_t
        {
        public:
            operator cv::InterpolationFlags() const;

        private:
            uint8_t m_value;
        };

        class LV_EnumCVInt_t
        {
        public:
            template <typename T>
            static int combine(LV_1DArrayHandle_t<T> handle)
            {
                static_assert(std::is_base_of<LV_EnumCVInt_t, T>::value, "Type is not derived from LV_EnumCVInt_t");

                // convert and accumulate flags
                return std::accumulate(handle.begin(), handle.end(), int(0), std::bit_or<int>());
            }

        protected:
            uint8_t m_value;
        };

#include "./reset_packing.hpp"
    }
}

#endif // G_AR_TK__INTEROP_LV_ENUMS_HPP_