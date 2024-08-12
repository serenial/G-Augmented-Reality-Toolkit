#ifndef G_AR_TK__INTEROP_LV_TERMINATION_CRITERIA_HPP_
#define G_AR_TK__INTEROP_LV_TERMINATION_CRITERIA_HPP_

#include <opencv2/core/types.hpp>

#include "./set_packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        // cv Termination Criteria
        using LV_TerminationCritera_t = struct
        {
            double epsilon;
            int32_t iterations;
        };

        cv::TermCriteria lv_termination_critera_ptr_to_cv_term_criteria(LV_TerminationCritera_t *);
    }
}

#include "./reset_packing.hpp"

#endif