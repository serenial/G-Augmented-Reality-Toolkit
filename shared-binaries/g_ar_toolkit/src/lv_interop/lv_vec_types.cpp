#include <g_ar_toolkit/lv_interop/lv_vec_types.hpp>

using namespace g_ar_toolkit;
using namespace lv_interop;

LV_ImagePointFloat_t::operator cv::Point2f() const
{
    return cv::Point2f{static_cast<float>(m_x), static_cast<float>(m_y)};
}

LV_ImagePointFloat_t::operator cv::Point2d() const
{
    return cv::Point2d{m_x, m_y};
}

LV_ImagePointFloat_t &LV_ImagePointFloat_t::operator=(const cv::Point &point)
{
    m_x = point.x;
    m_y = point.y;
    return *this;
}

LV_ImagePointFloat_t &LV_ImagePointFloat_t::operator=(const cv::Point2f &point)
{
    m_x = point.x;
    m_y = point.y;
    return *this;
}

LV_ImagePointFloat_t &LV_ImagePointFloat_t::operator=(const cv::Point2d &point)
{
    m_x = point.x;
    m_y = point.y;
    return *this;
}

LV_ImagePointFloat_t &LV_ImagePointFloat_t::operator=(const double array[2])
{
    m_x = array[0];
    m_y = array[1];
    return *this;
}

LV_ImagePointFloat_t &LV_ImagePointFloat_t::copy_from(const float *element_ptr)
{
    m_x = *element_ptr++;
    m_y = *element_ptr;
    return *this;
}

LV_ImagePointInt_t::operator cv::Point2i() const
{
    return cv::Point2i{m_x, m_y};
}

LV_ImagePointInt_t &LV_ImagePointInt_t::operator=(const cv::Point2i &point)
{
    m_x = point.x;
    m_y = point.y;
    return *this;
}

LV_ImagePointInt_t &LV_ImagePointInt_t::copy_from(const int *element_ptr)
{
    m_x = *element_ptr++;
    m_y = *element_ptr;
    return *this;
}

LV_ImageSize_t::operator cv::Size2i() const
{
    return cv::Size2i{m_width, m_height};
}

LV_ImageSize_t &LV_ImageSize_t::operator=(const cv::Size2i &size)
{
    m_width = size.width;
    m_height = size.height;
    return *this;
}

LV_WindowSize_t::operator cv::Size2i() const
{
    return cv::Size2i{m_width, m_height};
}

LV_Vec3Double_t::operator cv::Vec3d() const
{
    return cv::Vec3d{m_x, m_y, m_z};
}

LV_Vec3Double_t::operator cv::Scalar() const
{
    return cv::Scalar(m_x,m_y,m_z);
}

LV_Vec3Double_t::operator cv::Vec3f() const
{
    return cv::Vec3f{static_cast<float>(m_x), static_cast<float>(m_y), static_cast<float>(m_z)};
}

LV_Vec3Double_t::operator cv::Point3d() const
{
    return cv::Point3d{m_x, m_y, m_z};
}

LV_Vec3Double_t::operator cv::Point3f() const
{
    return cv::Point3f{static_cast<float>(m_x), static_cast<float>(m_y), static_cast<float>(m_z)};
}

cv::Mat LV_Vec3Double_t::as_cv_mat()
{
    return cv::Mat{1, 3, CV_64F, &m_x};
}

cv::Vec3d LV_Vec3Double_t::to_vec3d()
{
    return *this;
}

cv::Vec3f LV_Vec3Double_t::to_vec3f()
{
    return *this;
}

LV_Vec3Double_t::operator cv::Mat()
{
    return as_cv_mat();
}

LV_Vec3Double_t &LV_Vec3Double_t::operator=(const cv::Vec3d &v)
{
    m_x = v(0);
    m_y = v(1);
    m_z = v(2);
    return *this;
}
LV_Vec3Double_t &LV_Vec3Double_t::operator=(const cv::Vec3f &v)
{
    m_x = v(0);
    m_y = v(1);
    m_z = v(2);
    return *this;
}
LV_Vec3Double_t &LV_Vec3Double_t::operator=(const cv::Point3d &p)
{
    m_x = p.x;
    m_y = p.y;
    m_z = p.z;
    return *this;
}
LV_Vec3Double_t &LV_Vec3Double_t::operator=(const cv::Point3f &p)
{
    m_x = p.x;
    m_y = p.y;
    m_z = p.z;
    return *this;
}
LV_Vec3Double_t &LV_Vec3Double_t::operator=(const float a[3])
{
    m_x = a[0];
    m_y = a[1];
    m_z = a[2];
    return *this;
}