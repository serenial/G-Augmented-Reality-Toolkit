#pragma once

#include <vector>
#include <array>
#include <functional>
#include <cstring>
#include <numeric>
#include <exception>
#include <algorithm>
#include <cstddef>

#include <opencv2/core/mat.hpp>

#include "./lv_types.hpp"
#include "./lv_functions.hpp"
#include "./lv_array_1d.hpp"

#include "./set_packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        template <class T>
        class LV_2DArrayHandle_t
        {

        protected:
            LV_Handle_t<LV_Array_t<2, T>> m_handle;
            
        public:
            std::array<int32_t, 2> extents() const
            {
                if (m_handle && (*m_handle))
                {
                    return std::array<int32_t, 2>{{(*m_handle)->dims[0], (*m_handle)->dims[1]}};
                }
                return std::array<int32_t, 2>{{0, 0}};
            }

        private:
            size_t required_bytes(std::array<int32_t, 2> n_elements)
            {
                return LV_Array_t<2,T>::data_memeber_offset_bytes()+ sizeof(T) * n_elements[0] * n_elements[1];
            }

            int32_t get_data_index(std::array<int32_t, 2> el)
            {
                return extents()[1] * el[0] + el[1];
            }

            bool element_is_in_data_range(std::array<int32_t, 2> el)
            {
                return get_data_index < get_data_index(extents());
            }

        public:
            LV_2DArrayHandle_t() : m_handle(nullptr) {}

            T &operator[](std::array<int32_t, 2> el)
            {
                if (m_handle && element_is_in_data_range(el))
                {
                    return (*m_handle)->data[get_data_index(el)];
                }
                throw std::invalid_argument("Attempting to access LabVIEW Array Handle outside of bounds.");
            }

            T const &operator[](std::array<int32_t, 2> el) const
            {
                if (m_handle && element_is_in_data_range(el))
                {
                    return (*m_handle)->data[get_data_index(el)];
                }
                throw std::invalid_argument("Attempting to access LabVIEW Array Handle outside of bounds.");
            }

            T *data_handle() const noexcept
            {
                return (*m_handle)->data_ptr();
            }

            void dispose()
            {
                if (m_handle)
                {
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(m_handle));
                    m_handle = nullptr;
                }
            }

            void size_to_fit(std::array<int32_t, 2> n_elements, std::function<void(T)> deallocator = [](T el) {})
            {

                auto bytes = required_bytes(n_elements);
                auto uhandle = reinterpret_cast<LV_UHandle_t>(m_handle);

                if (DSCheckHandle(uhandle) == LV_ERR_mZoneErr)
                {
                    m_handle = reinterpret_cast<LV_Handle_t<LV_Array_t<2, T>>>(DSNewHandle(bytes));
                    (*m_handle)->dims[0] = n_elements[0];
                    (*m_handle)->dims[1] = n_elements[1];
                    return;
                }
                auto current_bytes = DSGetHandleSize(uhandle);

                if (bytes > current_bytes)
                {
                    auto err = DSSetHandleSize(uhandle, bytes);
                    if (err)
                    {
                        throw LV_MemoryManagerException(err);
                    }
                }

                auto old_extents = extents();

                for (int32_t row = n_elements[0]; row < old_extents[0]; row++)
                {
                    for (int32_t col = n_elements[1]; col < old_extents[1]; col++)
                    {
                        deallocator((*m_handle)->data[get_data_index({row, col})]);
                    }
                }

                (*m_handle)->dims[0] = n_elements[0];
                (*m_handle)->dims[1] = n_elements[1];
            }

            // copy from for cv::Mat
            void copy_memory_from(cv::Mat mat)
            {

                if (sizeof(T) != mat.elemSize())
                {
                    throw std::invalid_argument("cv Mat element size does not match the LV_2DArray_t element size.");
                }

                size_to_fit({mat.rows, mat.cols});

                if (mat.isContinuous())
                {
                    // copy block
                    std::memcpy(data_handle(), mat.data, mat.rows * mat.cols * sizeof(T));
                    return;
                }

                // copy row by row
                for (int32_t r = 0; r < mat.rows; r++)
                {
                    std::memcpy(data_handle() + get_data_index({r, 0}), mat.ptr(r, 0), mat.cols * sizeof(T));
                }
            }

            cv::Mat as_cv_mat(int type)
            {
                auto ex = extents();
                return cv::Mat{ex[0], ex[1], type, data_handle()};
            }
        };
    }
}

#include "./reset_packing.hpp"