#include <cstring>
#include <string>
#include <sstream>

#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

void LV_ErrorCluster_t::copy_from_exception(std::exception_ptr ex, const char * caller_name){

    std::stringstream ss;

    ss << caller_name << "<ERR>";

    try
    {
        if (ex)
        {
            std::rethrow_exception(ex);
        }
    }
    catch (cv::Exception &e)
    {
        ss << e.what();
        m_code = e.code;
    }
    catch (LV_MemoryManagerException &e)
    {
        ss << e.what();
        m_code = e.err;
    }
    catch (std::exception &e)
    {
        ss << e.what();
        m_code = LV_ERR_bogusError;
    }
    catch (...)
    {
        ss << "An undefined exception occured.";
        m_code = LV_ERR_bogusError;
    }

    m_status = m_code != 0;

    m_source.copy_from(ss.str());
}