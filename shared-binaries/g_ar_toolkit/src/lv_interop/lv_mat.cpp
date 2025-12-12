#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

cv::Mat LV_3x3Matrix_t::as_cv_mat()
{
    return cv::Mat{3, 3, CV_64F, &(m_value[0])};
}

LV_3x3Matrix_t::operator cv::Mat()
{
    return as_cv_mat();
}

eigen_map_3x3_t LV_3x3Matrix_t::as_eigen_map()
{
    return eigen_map_3x3_t(&(m_value[0]));
}

LV_3x3Matrix_t::operator eigen_map_3x3_t(){
    return as_eigen_map();
}

LV_3x3Matrix_t& LV_3x3Matrix_t::operator=(const Eigen::Matrix3d &m){
    
    // copy element by element into m_value as eigen is col-major
    for(int row =0; row<3; row++){
        for(int col=0; col<3; col++){
            m_value[row*3 + col] = m(row, col);
        }
    }

    return *this;
}

LV_3x3Matrix_t& LV_3x3Matrix_t::operator=(const cv::Mat &m){
        // copy element by element into m_value as eigen is col-major
        for(int row =0; row<3; row++){
            for(int col=0; col<3; col++){
                m_value[row*3 + col] = (row < m.rows && col < m.cols)? m.at<double>(row, col) : 0;
            }
        }
    
        return *this;
}

cv::Mat LV_2x3Matrix_t::as_cv_mat()
{
    return cv::Mat{2, 3, CV_64F, &(m_value[0])};
}

LV_2x3Matrix_t::operator cv::Mat()
{
    return as_cv_mat();
}

LV_4x4Matrix_t& LV_4x4Matrix_t::operator=(const Eigen::Matrix4f &m){
    
    // copy element by element into m_value as eigen is col-major
    for(int row =0; row<4; row++){
        for(int col=0; col<4; col++){
            m_value[row*4 + col] = m(row, col);
        }
    }

    return *this;
}