#ifndef G_AR_TK__INTEROP_LV_IMAGE_HPP_
#define G_AR_TK__INTEROP_LV_IMAGE_HPP_

#include <mutex>
#include <condition_variable>
#include <opencv2/core/mat.hpp>

#include "./lv_types.hpp"

namespace g_ar_toolkit
{
    using namespace lv_interop;

#include "./set_packing.hpp"
    struct LV_ImageSize_t
    {
        uint16_t width, height;
        cv::Size size();
    };
    using LV_ImageSizePtr_t = LV_Ptr_t<LV_ImageSize_t>;
#include "./reset_packing.hpp"

    // an Image class which can interact with EDVR references and the persistant data that they refer to
    class lv_image
    {
    public:
        // create a new edvr_ref and add the cv::Mat
        lv_image(LV_EDVRReferencePtr_t, cv::Size, bool);
        // convert an existing edvr_ref
        lv_image(LV_EDVRReferencePtr_t);
        // destructor
        ~lv_image();
        bool is_bgra();
        bool is_greyscale();
        cv::Mat const &mat();
        // add some cv::Mat methods for ease of use
        size_t width();
        size_t height();
        cv::Size size();
        cv::Mat clone();
        int cv_type();
        void copyTo(cv::_OutputArray);
        void copyTo(cv::_OutputArray, cv::_InputArray);
        void set_mat(cv::Mat);
        void ensure_sized_to_match(cv::Size);
        template <typename T>
        void ensure_sized_to_match(T x)
        {
            ensure_sized_to_match(x.size());
        }
        void upgrade_to_mapped();
        void downgrade_from_mapped();
        cv::Mat operator()(cv::Rect2i);

        // user-defined conversions to openCV's Input and Output Array types
        // this allows an Image instance to pretend to work like a cv::Mat
        operator cv::_InputArray() { return data->mat; }
        operator cv::_OutputArray() { return data->mat; }
        operator cv::Mat *() { return &(data->mat); }

    private:
        enum lock_state_t
        {
            NONE,
            LABVIEW,
            CPP,
            CPP_MAPPED
        };
        struct image_persistant_data
        {
            cv::Mat mat;
            lock_state_t locked;
            std::mutex m;
            std::condition_variable cv;
        };

        // private functions required for initialization
        LV_EDVRContext_t get_ctx();
        LV_EDVRDataPtr_t create_new_edvr_data_ptr();
        LV_EDVRDataPtr_t get_edvr_data_ptr();
        image_persistant_data *get_metadata();

        // locking and unlocking utility functions
        static inline void lock(image_persistant_data *, lock_state_t);
        static inline void unlock(image_persistant_data *, lock_state_t);

        // private properties - the order here is important for the initialization step
        const LV_EDVRReferencePtr_t edvr_ref_ptr;
        const LV_EDVRContext_t ctx;
        const LV_EDVRDataPtr_t edvr_data_ptr;
        image_persistant_data *const data;
    };
}

#endif //G_AR_TK__INTEROP_LV_IMAGE_HPP_