#pragma once
#include <opencv2/core/matx.hpp>

#include "./lv-types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

#include "./set-packing.hpp"
        template <size_t rows, size_t cols, typename T>
        using LV_FixedSizeMatrix_t = T[rows][cols];

        template <size_t rows, size_t cols, typename T>
        using LV_FixedSizeMatrixPtr_t = LV_Ptr_t<LV_FixedSizeMatrix_t<rows, cols, T>>;

        template <typename T>
        using LV_2x3MatrixPtr_t = LV_FixedSizeMatrixPtr_t<2,3,T>;

        template <typename T>
        using LV_3x3MatrixPtr_t = LV_FixedSizeMatrixPtr_t<3,3,T>;

#include "./reset-packing.hpp"

template <size_t rows, size_t cols, typename T>
cv::Matx<T,rows,cols> cv_matx_from_lv_fixed_sized_matrix_ptr(LV_FixedSizeMatrixPtr_t<rows,cols,T> input){
    return cv::Matx<T,rows,cols>(reinterpret_cast<T*>(input));
}
    }
}