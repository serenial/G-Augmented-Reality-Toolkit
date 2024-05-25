#include <cstring>
#include <opencv2/core.hpp>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_functions.hpp"
#include "g_ar_toolkit/lv_interop/lv_array.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

LV_MgErr_t lv_interop::write_to_lv_error_cluster_ptr(LV_ErrorClusterPtr_t lv_err_ptr, LV_MgErr_t code, const char *current_location, const char *text)
{

    // set boolean status flag and code
    lv_err_ptr->status = code != 0;
    lv_err_ptr->code = code;

    size_t text_length = std::strlen(text);

    const char *text_prepend = "<ERR>";
    size_t text_prepend_length = text_length ? std::strlen(text_prepend) : 0;

    size_t current_location_length = std::strlen(current_location);

    size_t required_length = current_location_length + text_prepend_length + text_length;

    // size source string-handle to hold error source message
    ensure_array_handle_ptr_can_hold_n_elements(&(lv_err_ptr->source), required_length);

    // copy function name, prepend and text into string handle
    std::memcpy((*lv_err_ptr->source)->data, current_location, current_location_length);
    std::memcpy((*lv_err_ptr->source)->data + current_location_length, text_prepend, text_prepend_length);
    std::memcpy((*lv_err_ptr->source)->data + current_location_length + text_prepend_length, text, text_length);

    (*lv_err_ptr->source)->dims[0] = required_length;

    return LV_ERR_noError;
}

LV_MgErr_t lv_interop::write_undefined_err_to_lv_error_cluster_ptr(LV_ErrorClusterPtr_t lv_err_ptr, const char *current_location)
{
    return write_to_lv_error_cluster_ptr(lv_err_ptr, LV_ERR_bogusError, current_location, "An undefined exception occured.");
}


LV_MgErr_t lv_interop::caught_exception_to_lv_err(std::exception_ptr eptr,LV_ErrorClusterPtr_t lv_err_ptr, const char* current_location){
    try
    {
        if (eptr)
            std::rethrow_exception(eptr);
    }
        catch (cv::Exception &e)
        {
            return write_to_lv_error_cluster_ptr(lv_err_ptr, e.code, current_location, e.what());
        }
        catch (LV_MemoryManagerException &e){
             return write_to_lv_error_cluster_ptr(lv_err_ptr, e.err, current_location, e.what());
        }
        catch (std::exception &e)
        {
            return write_to_lv_error_cluster_ptr(lv_err_ptr, LV_ERR_bogusError, current_location, e.what());
        }
        catch (...)
        {
            return write_undefined_err_to_lv_error_cluster_ptr(lv_err_ptr, current_location);
        }

        return LV_ERR_noError;
}