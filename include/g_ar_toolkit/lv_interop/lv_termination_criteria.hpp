#ifndef G_AR_TK__INTEROP_LV_TERMINATION_CRITERIA_HPP_
#define G_AR_TK__INTEROP_LV_TERMINATION_CRITERIA_HPP_

#include <opencv2/core/types.hpp>

#include "./set_packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        // cv Termination Criteria
        struct LV_TerminationCritera_t
        {
            double m_epsilon;
            int32_t m_iterations;
            operator cv::TermCriteria() const;
        };
    }
}

#include "./reset_packing.hpp"

#endif