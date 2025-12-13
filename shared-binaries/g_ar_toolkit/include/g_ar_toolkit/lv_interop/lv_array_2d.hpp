#pragma once

#include <vector>
#include <array>
#include <functional>
#include <cstring>
#include <numeric>
#include <exception>
#include <algorithm>

#include <opencv2/core/mat.hpp>

#include "./lv_types.hpp"
#include "./lv_functions.hpp"
#include "./lv_array_md.hpp"

#include "./set_packing.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        template <class T>
        class LV_2DArrayHandle_t : public LV_MDArrayHandle_t<2,T>
        {
        public:
            LV_2DArrayHandle_t() = delete;

            cv::Size size() const
            {
                auto dims = this->extents();
                return cv::Size(dims[1], dims[0]);
            }

            // copy from for cv::Mat
            void copy_memory_from(const cv::Mat &mat)
            {

                if (sizeof(T) != mat.elemSize())
                {
                    throw std::invalid_argument("cv Mat element size does not match the LV_2DArray_t element size.");
                }

                this->size_to_fit({mat.rows, mat.cols});

                if (mat.isContinuous())
                {
                    // copy block
                    std::memcpy(this->begin(), mat.data, mat.rows * mat.cols * sizeof(T));
                    return;
                }

                // copy row by row
                for (int32_t r = 0; r < mat.rows; r++)
                {
                    std::memcpy(this->at({r, 0}), mat.ptr(r, 0), mat.cols * sizeof(T));
                }
            }

            cv::Mat as_cv_mat(int type)
            {
                auto ex = this->extents();
                return cv::Mat{ex[0], ex[1], type, this->begin()};
            }
        };
    }
}

#include "./reset_packing.hpp"