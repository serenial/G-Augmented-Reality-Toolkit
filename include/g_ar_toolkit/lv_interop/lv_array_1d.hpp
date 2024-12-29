#pragma once

#include <vector>
#include <array>
#include <functional>
#include <cstring>
#include <numeric>
#include <exception>
#include <algorithm>
#include <stdexcept>

#include <opencv2/core/mat.hpp>

#include "./lv_types.hpp"
#include "./lv_functions.hpp"


#include "./set_packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        template <size_t n_dims, typename T>
        struct LV_Array_t
        {
            int32_t dims[n_dims];
            T data[1];

            T *data_ptr()
            {
                return reinterpret_cast<T *>(&data[0]);
            }
        };

        template <class T>
        class LV_1DArrayHandle_t
        {

        protected:
            LV_Handle_t<LV_Array_t<1, T>> m_handle;

        private:
            size_t required_bytes(size_t n_elements)
            {
                size_t data_header_size = reinterpret_cast<uintptr_t>((*m_handle)->data_ptr()) - reinterpret_cast<uintptr_t>(*m_handle);
                return data_header_size + sizeof(T) * n_elements;
            }

        public:
            LV_1DArrayHandle_t() : m_handle(nullptr) {}
            
            bool empty() const {
                return size() == 0;
            }

            T &operator[](int i)
            {
                if (m_handle && i < size())
                {
                    return (*m_handle)->data[i];
                }
                throw std::invalid_argument("Attempting to access LabVIEW Array Handle outside of bounds.");
            }

            T const &operator[](int i) const
            {
                if (m_handle && i < size())
                {
                    return (*m_handle)->data[i];
                }
                throw std::invalid_argument("Attempting to access LabVIEW Array Handle outside of bounds.");
            }

            std::size_t size() const
            {
                return m_handle && (*m_handle) ? (*m_handle)->dims[0] : 0;
            }

            std::size_t is_nullptr() const
            {
                return m_handle == nullptr;
            }

            T *begin() const noexcept
            {
                return m_handle ? (*m_handle)->data_ptr() : nullptr;
            }

            T *end() const noexcept
            {
                return m_handle ? (*m_handle)->data_ptr() + size() : nullptr;
            }

            void dispose()
            {
                if (m_handle)
                {
                    DSDisposeHandle(reinterpret_cast<LV_UHandle_t>(m_handle));
                    m_handle = nullptr;
                }
            }

            void size_to_fit(size_t n_elements, std::function<void(T)> deallocator = [](T el) {})
            {

                auto bytes = required_bytes(n_elements);
                auto uhandle = reinterpret_cast<LV_UHandle_t>(m_handle);

                if (DSCheckHandle(uhandle) == LV_ERR_mZoneErr)
                {
                    m_handle = reinterpret_cast<LV_Handle_t<LV_Array_t<1, T>>>(DSNewHandle(bytes));
                    (*m_handle)->dims[0] = n_elements;
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

                if (begin() + n_elements < end())
                {
                    std::for_each(begin() + n_elements, end(), deallocator);
                }

                (*m_handle)->dims[0] = n_elements;
            }

            // copy from for non fundamental types that requires each element to be copied in turn
            template <class ElementType, typename CopyFunction = std::function<void(const ElementType &, T *)>>
            void copy_element_by_element_from(std::vector<ElementType> vector, CopyFunction copy_fn = [](auto from, auto to){ *to = from;}, std::function<void(T)> deallocator = [](T el) {})
            {
                size_to_fit(vector.size(), deallocator);

                auto vector_it = vector.begin();
                auto array_it = begin();

                for (; vector_it != vector.end() && array_it != end(); ++vector_it, ++array_it)
                {
                    copy_fn(*vector_it, array_it);
                }
            }

            // copy from for contiguous fundamental types which can be copied in a block
            template <class ElementType>
            void copy_memory_from(std::vector<ElementType> vector)
            {
                // memcpy only works with fundamental types
                static_assert(std::is_fundamental_v<ElementType> == true, "Default copying is only compatible for fundamental value types. Specify the copy function [](auto from, auto to){ /* do conversion */}");
                size_to_fit(vector.size());
                std::memcpy(begin(), vector.data(), vector.size() * sizeof(T));
            }

            // copy from for cv::Mat
            void copy_memory_from(cv::Mat mat)
            {

                size_t n_elements = mat.rows * mat.cols * mat.channels();

                if (sizeof(T) != mat.elemSize())
                {
                    throw std::invalid_argument("cv Mat element size does not match the LV_1DArray_t element size.");
                }

                size_to_fit(n_elements);

                if (mat.isContinuous())
                {
                    // copy block
                    std::memcpy(begin(), mat.data, n_elements * sizeof(T));
                    return;
                }

                // copy row by row
                for (size_t r = 0; r < mat.rows; r++)
                {
                    std::memcpy(begin(), mat.ptr(r, 0), mat.cols * sizeof(T));
                }
            }

            template <typename T_vec>
            std::vector<T_vec> to_vector() const
            {

                std::vector<T_vec> output_vector;

                output_vector.reserve(size());

                for (const auto &e : *this)
                {
                    output_vector.push_back(e);
                }

                return output_vector;
            }

            template <typename T_vec>
            std::vector<T_vec> as_vector() const
            {

                std::vector<T_vec> output_vector;

                output_vector.reserve(size());
                // note the lack of const
                for (auto &e : *this)
                {
                    output_vector.push_back(e);
                }

                return output_vector;
            }
        };
    }
}

#include "./reset_packing.hpp"
