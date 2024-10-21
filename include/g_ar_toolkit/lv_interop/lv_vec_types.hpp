#ifndef G_AR_TK__INTEROP_LV_VEC_TYPES_HPP_
#define G_AR_TK__INTEROP_LV_VEC_TYPES_HPP_

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>

#include "./lv_types.hpp"
#include "./lv_array_1d.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

#include "./set_packing.hpp"
        
        struct LV_ImagePointFloat_t{
            double m_x,m_y;
            operator cv::Point2f() const;
            operator cv::Point2d() const;
            LV_ImagePointFloat_t& operator=(const cv::Point2f &);
            LV_ImagePointFloat_t& operator=(const cv::Point2d &);
            LV_ImagePointFloat_t& operator=(const double[2]);
        };

        struct LV_ImagePointInt_t{
            int32_t m_x,m_y;
            operator cv::Point2i() const;
            LV_ImagePointInt_t& operator=(const cv::Point2i &);
        };


        struct LV_ImageSize_t{
            uint16_t m_width,m_height;
            operator cv::Size2i() const;
            LV_ImageSize_t& operator= (const cv::Size2i&);
        };

        struct LV_WindowSize_t{
            int32_t m_width,m_height;
            operator cv::Size2i() const;
        };

        struct LV_Vec3Double_t{
            double m_x,m_y,m_z;
            operator cv::Vec3d() const;
            operator cv::Vec3f() const;
            operator cv::Point3d() const;
            operator cv::Point3f() const;
            operator cv::Mat();
            cv::Mat as_cv_mat();
            cv::Vec3d to_vec3d();
            cv::Vec3f to_vec3f();
            LV_Vec3Double_t& operator=(const cv::Vec3d&);
            LV_Vec3Double_t& operator=(const cv::Vec3f&);
            LV_Vec3Double_t& operator=(const cv::Point3d&);
            LV_Vec3Double_t& operator=(const cv::Point3f&);
            LV_Vec3Double_t& operator=(const float[3]);
            
        };


using LV_ImagePointFloatPtr_t = LV_Ptr_t<LV_ImagePointFloat_t>;
using LV_ImagePointIntPtr_t = LV_Ptr_t<LV_ImagePointInt_t>;
using LV_ImageSizePtr_t = LV_Ptr_t<LV_ImageSize_t>;
using LV_WindowSizePtr_t = LV_Ptr_t<LV_WindowSize_t>;

using LV_RVec_t = LV_Vec3Double_t;
using LV_TVec_t = LV_Vec3Double_t;
using LV_RVecPtr_t = LV_Ptr_t<LV_RVec_t>;
using LV_TVecPtr_t = LV_Ptr_t<LV_TVec_t>;

using LV_ObjectPointFloat_t = LV_Vec3Double_t;
using LV_ObjectPointFloatPtr_t = LV_Ptr_t<LV_ObjectPointFloat_t>;

#include "./reset_packing.hpp"
    }

}

#endif // G_AR_TK__INTEROP_LV_VEC_TYPES_HPP_