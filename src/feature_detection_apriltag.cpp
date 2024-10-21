#include <utility>
#include <functional>

#include <apriltag.h>
#include <tag16h5.h>
#include <tag25h9.h>
#include <tag36h10.h>
#include <tag36h11.h>
#include <tagCircle21h7.h>
#include <tagCircle49h12.h>
#include <tagCustom48h12.h>
#include <tagStandard41h12.h>
#include <tagStandard52h13.h>
#include <common/image_u8.h>
#include <common/zarray.h>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_image.hpp"
#include "g_ar_toolkit/lv_interop/lv_edvr_managed_object.hpp"
#include "g_ar_toolkit/lv_interop/lv_enums.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_export.h"

using namespace g_ar_toolkit;
using namespace lv_interop;

namespace
{
#include "g_ar_toolkit/lv_interop/set_packing.hpp"

    using LV_AprilTagFamily_t = uint8_t;

    class LV_ApriltagDetection_t
    {
    private:
        int32_t m_id;
        int32_t m_hamming;
        float m_decision_margin;
        LV_ImagePointFloat_t m_centre, m_bottom_left, m_bottom_right, m_top_right, m_top_left;

    public:
        LV_ApriltagDetection_t &operator=(const apriltag_detection_t *detection)
        {
            m_id = detection->id;
            m_hamming = detection->hamming;
            m_decision_margin = detection->decision_margin;
            m_centre = detection->c;
            m_bottom_left = detection->p[0];
            m_bottom_right = detection->p[1];
            m_top_right = detection->p[2];
            m_top_left = detection->p[3];
            return *this;
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    class Tag
    {
    private:
        std::function<void(apriltag_family_t *)> m_deallocate_family_fn;
        apriltag_family_t *m_tf;

    public:
        Tag(Tag const &) = delete;
        void operator=(Tag const &x) = delete;
        Tag(LV_AprilTagFamily_t family)
        {
            const std::pair<std::function<apriltag_family_t *()>, std::function<void(apriltag_family_t *)>> families[]{
                std::make_pair(tag16h5_create, tag16h5_destroy),
                std::make_pair(tag25h9_create, tag25h9_destroy),
                std::make_pair(tag36h11_create, tag36h11_destroy),
                std::make_pair(tagCircle21h7_create, tagCircle21h7_destroy),
                std::make_pair(tagCircle49h12_create, tagCircle49h12_destroy),
                std::make_pair(tagCustom48h12_create, tagCustom48h12_destroy),
                std::make_pair(tagStandard41h12_create, tagStandard41h12_destroy),
                std::make_pair(tagStandard52h13_create, tagStandard52h13_destroy),
            };

            if (family >= std::size(families))
            {
                throw std::out_of_range("The supplied value for the tag family does not match an Apriltag tag-family.");
            }

            m_tf = families[family].first();
            m_deallocate_family_fn = families[family].second;
        }

        void add_to_detector(apriltag_detector_t *detector, int bits)
        {
            if (m_tf != nullptr)
            {
                apriltag_detector_add_family_bits(detector, m_tf, bits);
            }
        }

        ~Tag()
        {
            if (m_tf != nullptr)
            {
                m_deallocate_family_fn(m_tf);
                m_tf = nullptr;
            }
        }
    };

    class TagDetector
    {
    private:
        apriltag_detector_t *m_detector;
        Tag m_tag_family;
        zarray_t *m_detections;

    public:
        TagDetector(TagDetector const &) = delete;
        void operator=(TagDetector const &x) = delete;
        TagDetector(LV_AprilTagFamily_t family, int bits, float decimate, float blur, int32_t threads, bool refine_edges) : m_tag_family(Tag(family)),
                                                                                                                            m_detector(apriltag_detector_create()),
                                                                                                                            m_detections(nullptr)
        {
            m_tag_family.add_to_detector(m_detector, bits);
            m_detector->quad_decimate = decimate;
            m_detector->quad_sigma = blur;
            m_detector->nthreads = threads;
            m_detector->refine_edges = refine_edges;
        }

        ~TagDetector()
        {
            if (m_detections != nullptr)
            {
                apriltag_detections_destroy(m_detections);
            }
            apriltag_detector_destroy(m_detector);
        }

        void detect_tags(const cv::Mat &image, LV_1DArrayHandle_t<LV_ApriltagDetection_t> handle)
        {
            image_u8_t apriltag_image {image.cols,image.rows,image.cols,image.data};

            if (m_detections != nullptr)
            {
                apriltag_detections_destroy(m_detections);
            }

            m_detections = apriltag_detector_detect(m_detector, &apriltag_image);

            auto n_detections = zarray_size(m_detections);

            handle.size_to_fit(n_detections);

            auto handle_element = handle.begin();
            for (size_t i = 0; i < n_detections; ++i, ++handle_element)
            {

                apriltag_detection_t *detection;
                zarray_get(m_detections, i, &detection);

                *handle_element = detection;
            }
        }
    };
}

extern "C"
{
    G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_apriltag_create_detector(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        const LV_AprilTagFamily_t tag_family,
        const uint8_t bits,
        const float decimate,
        const float blur, 
        const uint8_t threads, 
        const LV_BooleanPtr_t refine_edges_ptr,
        LV_EDVRReferencePtr_t edvr_ref_ptr)
    {
        try
        {
            volatile EDVRManagedObject<TagDetector> tag_detector(edvr_ref_ptr, new TagDetector(tag_family, bits, decimate, blur, threads, refine_edges_ptr));
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }

        G_AR_TOOLKIT_EXPORT LV_MgErr_t g_ar_tk_fd_apriltag_detect_tags(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_EDVRReferencePtr_t detector_ref_ptr,
        LV_EDVRReferencePtr_t src_edvr_ref_ptr,
        LV_1DArrayHandle_t<LV_ApriltagDetection_t> detections_handle)
    {
        try
        {
            EDVRManagedObject<TagDetector> tag_detector(detector_ref_ptr);

            lv_image image(src_edvr_ref_ptr);

            if(image.is_bgra()){
                throw std::invalid_argument("Image should be greyscale.");
            }

            tag_detector.get_object()->detect_tags(image, detections_handle);
            
        }
        catch (...)
        {
            error_cluster_ptr->copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}