#include <cmath>
#include <bitset>

#include "g_ar_toolkit/lv_interop/lv_termination_criteria.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

LV_TerminationCritera_t::operator cv::TermCriteria() const
{

    std::bitset<2> type = 0b00;

    if (std::isnan(m_epsilon))
    {
        type |= 0b01;
    }

    if (m_iterations >= 0)
    {
        type |= 0b10;
    }

    switch (type.to_ulong())
    {
    case 0b01:
        return cv::TermCriteria(cv::TermCriteria::EPS, 0, m_epsilon);
    case 0b10:
        return cv::TermCriteria(cv::TermCriteria::COUNT, m_iterations, 0);
    case 0b11:
        return cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, m_iterations, m_epsilon);
    }

    return cv::TermCriteria();
}