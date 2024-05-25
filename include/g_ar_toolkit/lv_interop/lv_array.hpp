#ifndef G_AR_TK__INTEROP_LV_ARRAY_HPP_
#define G_AR_TK__INTEROP_LV_ARRAY_HPP_

#include <vector>
#include <array>
#include <functional>
#include <cstring>
#include <numeric>
#include "./lv_types.hpp"
#include "./lv_functions.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        template <typename T, size_t n_dims>
        void ensure_array_handle_ptr_can_hold_n_elements(LV_HandlePtr_t<LV_Array_t<n_dims, T>> array_handle_ptr, size_t dims[n_dims])
        {
            // get the total number of elements as the product of the required_dims
            size_t total_elements = std::accumulate(dims, dims + n_dims, size_t(1), std::multiplies<size_t>());

            // size in bytes is array-struct which is
            size_t required_bytes = sizeof(LV_Array_t<n_dims, T>) + sizeof(T) * (total_elements - 1);
            auto handle_ptr = reinterpret_cast<LV_UHandle_t *>(array_handle_ptr);

            if (DSCheckHandle(*handle_ptr) == LV_ERR_mZoneErr)
            {
                // new handle needs to be sized
                *handle_ptr = DSNewHandle(required_bytes);
                return;
            }

            if (required_bytes < DSGetHandleSize(*handle_ptr))
            {
                // handle already large enough
                return;
            }

            auto err = DSSetHandleSize(*handle_ptr, required_bytes);
            if (err)
            {
                throw LV_MemoryManagerException(err);
            }
        }

        template <typename T>
        void ensure_array_handle_ptr_can_hold_n_elements(LV_HandlePtr_t<LV_Array_t<1, T>> array_handle_ptr, size_t n_elements)
        {
            size_t n[1] = {n_elements};
            ensure_array_handle_ptr_can_hold_n_elements<T, 1>(array_handle_ptr, n);
        }

        template <typename T>
        auto array_handle_ptr_array_length(LV_HandlePtr_t<LV_Array_t<1, T>> handle_ptr)
        {
            if (!handle_ptr || !(*handle_ptr) || !(**handle_ptr))
            {
                return 0;
            }
            return (**handle_ptr)->dims[0];
        }

        template <typename T>
        auto array_handle_ptr_array_length(LV_HandlePtr_t<LV_Array_t<2, T>> handle_ptr)
        {
            if (!handle_ptr || !(*handle_ptr) || !(**handle_ptr))
            {
                return std::array<size_t,2>{ 0, 0 };
            }
            return (**handle_ptr)->dims;
        }

        template <typename T, typename U>
        void copy_with_allocation_to_1d_lv_array_handle_ptr(
            const T &collection,
            LV_HandlePtr_t<LV_Array_t<1, U>> handle_ptr,
            std::function<void(U *, bool, typename T::value_type)> copy = [](auto *out, bool, auto in)
            { *out = in; },
            std::function<void(U)> deallocate = [](auto for_deallocation) {})
        {
            size_t handle_start_len = array_handle_ptr_array_length(handle_ptr);

            ensure_array_handle_ptr_can_hold_n_elements(handle_ptr, collection.size());

            auto data_ptr = (**handle_ptr)->data_ptr();

            // transform and copy each element
            for (auto &element : collection)
            {
                auto is_newly_allocated = (data_ptr - (**handle_ptr)->data_ptr()) >= handle_start_len;
                copy(data_ptr, is_newly_allocated, element);
                data_ptr++;
            }

            // deallocate any unused elements from array
            while (data_ptr < ((**handle_ptr)->data_ptr() + handle_start_len))
            {
                deallocate(*data_ptr);
                data_ptr++;
            }

            // set the new size of the array
            (**handle_ptr)->dims[0] = collection.size();
        }

        template <typename U, typename T>
        void copy_1d_lv_array_handle_to_vector(
            LV_Handle_t<LV_Array_t<1, U>> handle,
            std::vector<T> &vector,
            std::function<T(U)> transform)
        {
            size_t n_elements = (handle && *handle) ? (*handle)->dims[0] : 0;

            vector.reserve(n_elements + vector.size());

            auto data_ptr = (*handle)->data_ptr();

            for (size_t i = 0; i < n_elements; i++)
            {
                vector.push_back(transform(*data_ptr));
                data_ptr++;
            }
        }
    }
}

#endif // G_AR_TK__INTEROP_LV_ARRAY_HPP_