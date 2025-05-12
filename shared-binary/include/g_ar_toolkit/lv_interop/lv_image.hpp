#pragma once

#include <mutex>
#include <condition_variable>
#include <opencv2/core/mat.hpp>
#include <ZXing/ImageView.h>

#include "./lv_types.hpp"

namespace g_ar_toolkit
{
    using namespace lv_interop;

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
        bool is_bgra() const;
        bool is_greyscale() const;
        cv::Mat const &mat() const;
        // add some cv::Mat methods for ease of use
        size_t width() const;
        size_t height() const;
        cv::Size size() const;
        cv::Mat clone() const;
        int cv_type() const;
        void copyTo(cv::_OutputArray) const;
        void copyTo(cv::_OutputArray, cv::_InputArray) const;
        void set_mat(cv::Mat);
        void ensure_sized_to_match(const cv::Size &);
        void ensure_sized_to_match(const lv_image &);
        void upgrade_to_mapped();
        void downgrade_from_mapped();
        cv::Mat operator()(cv::Rect2i) const;

        template <class T, class... Targs>
        T &at(Targs... arguments)
        {
            return data->mat.at<T>(arguments...);
        }

        // user-defined conversions to openCV's Input and Output Array types
        // this allows an Image instance to pretend to work like a cv::Mat
        operator cv::_InputArray();
        operator cv::_OutputArray();
        operator cv::Mat *();
        operator const cv::Mat &() const;
        operator cv::Mat &();
        operator const ZXing::ImageView();

    private:
        struct image_persistant_data_t
        {
            enum lock_states
            {
                NONE,
                LABVIEW,
                CPP,
                CPP_MAPPED
            };
            cv::Mat mat;
            lock_states locked;
            std::mutex m;
            std::condition_variable cv;

            // locking and unlocking utility functions
            static void lock(image_persistant_data_t *, lock_states);
            static void unlock(image_persistant_data_t *, lock_states);
        };

        // private functions required for initialization
        LV_EDVRContext_t get_ctx();
        LV_EDVRDataPtr_t create_new_edvr_data_ptr();
        LV_EDVRDataPtr_t get_edvr_data_ptr();
        image_persistant_data_t *get_metadata();

        // private properties - the order here is important for the initialization step
        const LV_EDVRReferencePtr_t edvr_ref_ptr;
        const LV_EDVRContext_t ctx;
        const LV_EDVRDataPtr_t edvr_data_ptr;
        image_persistant_data_t *const data;

        static LV_MgErr_t on_labview_lock(LV_EDVRDataPtr_t ptr);
        static LV_MgErr_t on_labview_unlock(LV_EDVRDataPtr_t ptr);
        static void on_labview_delete(LV_EDVRDataPtr_t ptr);
    };
}