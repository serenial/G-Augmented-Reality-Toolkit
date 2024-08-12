#include <cmath>
#include <bitset>

#include "g_ar_toolkit/lv_interop/lv_termination_criteria.hpp"

cv::TermCriteria g_ar_toolkit::lv_interop::lv_termination_critera_ptr_to_cv_term_criteria(LV_TerminationCritera_t *ptr)
{

    std::bitset<2> type = 0b00;

    if (std::isnan(ptr->epsilon))
    {
        type |= 0b01;
    }

    if (ptr->iterations >= 0)
    {
        type |= 0b10;
    }

    switch (type.to_ulong())
    {
    case 0b01:
        return cv::TermCriteria(cv::TermCriteria::EPS, 0, ptr->epsilon);
    case 0b10:
        return cv::TermCriteria(cv::TermCriteria::COUNT, ptr->iterations, 0);
    case 0b11:
        return cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, ptr->iterations, ptr->epsilon);
    }

    return cv::TermCriteria();
}