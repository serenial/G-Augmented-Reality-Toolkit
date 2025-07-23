#include "g_ar_toolkit/lv_interop/lv_point_correspondence.hpp"

using namespace g_ar_toolkit;
using namespace lv_interop;

std::vector<cv::Point2f> LV_PointCorrespondence_t::image_points_vec() const{
    std::vector<cv::Point2f> points;

    points.reserve(m_image_points_handle.size());

    for(const auto & p: m_image_points_handle){
        points.push_back(p);
    }

    return points;
}

std::vector<cv::Point3f> LV_PointCorrespondence_t::object_points_vec() const{
    std::vector<cv::Point3f> points;

    points.reserve(m_object_points_handle.size());

    for(const auto & p: m_object_points_handle){
        points.push_back(p);
    }

    return points;
}

size_t LV_PointCorrespondence_t::number_of_points() const{
    auto size = m_image_points_handle.size();

    if(m_object_points_handle.size() != size){
        throw std::invalid_argument("The number of Object Points and the number of Image Points do not match.");
    }

    return size;
}

std::vector<std::vector<cv::Point2f>> LV_PointCorrespondencesArrayHandle_t::image_points_vecs() const{

    std::vector<std::vector<cv::Point2f>> points_vecs;

    points_vecs.reserve(m_point_sets.size());

    for(const auto & p: m_point_sets){
        points_vecs.push_back(p.image_points_vec());
    }

    return points_vecs;
}

std::vector<std::vector<cv::Point3f>> LV_PointCorrespondencesArrayHandle_t::object_points_vecs() const{

    std::vector<std::vector<cv::Point3f>> points_vecs;

    points_vecs.reserve(m_point_sets.size());

    for(const auto & p: m_point_sets){
        points_vecs.push_back(p.object_points_vec());
    }

    return points_vecs;
}