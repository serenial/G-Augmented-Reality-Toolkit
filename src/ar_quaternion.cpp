#include <limits>

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"
#include "g_ar_toolkit/lv_interop/lv_mat.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    class LV_Quaternion_t
    {
    public:
        LV_Quaternion_t() = delete;
        LV_Quaternion_t(double w, double x, double y, double z);
        operator Eigen::Quaterniond() const;
        operator Eigen::Vector4d() const;
        Eigen::Quaterniond to_eigen_quaterniond() const;
        LV_Quaternion_t &operator=(Eigen::Quaterniond);
        LV_Quaternion_t &operator=(Eigen::Vector4d);

    private:
        double m_w, m_x, m_y, m_z;
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    using LV_QuaternionPtr_t = LV_Ptr_t<LV_Quaternion_t>;

}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_rotation_matrix_to_quaternion(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_3x3MatrixPtr_t rotation_mat_ptr,
        LV_QuaternionPtr_t quaternion_ptr)
    {
        try
        {
            *quaternion_ptr = Eigen::Quaterniond(rotation_mat_ptr->as_eigen_map()).normalized();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_quaternion_to_rotation_matrix(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_QuaternionPtr_t quaternion_ptr,
        LV_3x3MatrixPtr_t rotation_mat_ptr)
    {
        try
        {
            *rotation_mat_ptr = quaternion_ptr->to_eigen_quaterniond().toRotationMatrix();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_quaternion_slerp(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_QuaternionPtr_t from_ptr,
        LV_QuaternionPtr_t to_ptr,
        double factor,
        LV_QuaternionPtr_t result_ptr)
    {
        try
        {
            *result_ptr = from_ptr->to_eigen_quaterniond().slerp(factor, to_ptr->to_eigen_quaterniond()).normalized();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_ar_quaternion_average(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_1DArrayHandle_t<LV_Quaternion_t> input_handle,
        LV_1DArrayHandle_t<double> weights_handle,
        LV_QuaternionPtr_t result_ptr)
    {
        try
        {
            if (input_handle.empty())
            {
                double NaN = std::numeric_limits<double>::quiet_NaN();
                *result_ptr = LV_Quaternion_t(NaN, NaN, NaN, NaN);
                throw std::invalid_argument("Number of samples must be greater than zero.");
            }

            // see https://tbirdal.blogspot.com/2019/10/i-allocate-this-post-to-providing.html for more information
            // this code is based on https://github.com/tolgabirdal/averaging_quaternions released under the MIT licence

            Eigen::Matrix4d accumulator = Eigen::Matrix4d::Zero();

            double summed_weights(0);

            for (int i = 0; i < input_handle.size(); i++)
            {
                // working with vec so x,y,z,w
                Eigen::Vector4d vec = input_handle[i];

                vec.normalize();

                // handle flipped (w<0)
                if(vec(3)<0){
                    vec = -vec;
                }

                double weight = (i < weights_handle.size()) ? weights_handle[i] : 1.0;
                summed_weights += weight;

                // accumulate outer-vector-product
                accumulator += weight * (vec * vec.adjoint());
            }

            Eigen::SelfAdjointEigenSolver<Eigen::Matrix4d> solver(accumulator / summed_weights);

            *result_ptr = solver.eigenvectors().col(3).normalized();
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}

LV_Quaternion_t &LV_Quaternion_t::operator=(Eigen::Quaterniond q)
{
    m_w = q.w();
    m_x = q.x();
    m_y = q.y();
    m_z = q.z();
    return *this;
}

Eigen::Quaterniond LV_Quaternion_t::to_eigen_quaterniond() const
{
    Eigen::Quaterniond q(m_w, m_x, m_y, m_z);
    q.normalize();
    return q;
}

LV_Quaternion_t::operator Eigen::Quaterniond() const
{
    return to_eigen_quaterniond();
}

LV_Quaternion_t::LV_Quaternion_t(double w, double x, double y, double z) : m_w(w), m_x(x), m_y(y), m_z(z)
{
}

LV_Quaternion_t::operator Eigen::Vector4d() const{
    // return as a Vector4d in the "internal" quaternion order!
    // https://eigen.tuxfamily.org/dox/classEigen_1_1Quaternion.html#a82d58d74e9c895715b24807c9096296f
    return Eigen::Vector4d(m_x, m_y, m_z, m_w);
}

LV_Quaternion_t& LV_Quaternion_t::operator=(Eigen::Vector4d vec){
    // Vector4d in the "internal" quaternion order!
    // https://eigen.tuxfamily.org/dox/classEigen_1_1Quaternion.html#a82d58d74e9c895715b24807c9096296f
    m_x = vec(0);
    m_y = vec(1);
    m_z = vec(2);
    m_w = vec(3);

    return *this;
}

