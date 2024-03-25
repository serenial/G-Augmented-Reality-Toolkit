#pragma once

#include <mutex>
#include <condition_variable>
#include <opencv2/core/mat.hpp>

#include "g_ar_toolkit/lv-interop/lv-types.hpp"

namespace g_ar_toolkit
{
    using namespace lv_interop;

    namespace image
    {

#include "g_ar_toolkit/lv-interop/set-packing.hpp"
        struct LV_ImageSize_t
        {
            uint16_t width, height;
            cv::Size size();
        };
        using LV_ImageSizePtr_t = LV_Ptr_t<LV_ImageSize_t>;
#include "g_ar_toolkit/lv-interop/reset-packing.hpp"

        // an Image class which can interact with EDVR references and the persistant data that they refer to
        class Image
        {
        public:
            // create a new edvr_ref and add the cv::Mat
            Image(LV_EDVRReferencePtr_t, cv::Size, bool);
            // convert an existing edvr_ref
            Image(LV_EDVRReferencePtr_t);
            // destructor
            ~Image();
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
            template<typename T>
            void ensure_sized_to_match(T x){
                ensure_sized_to_match(x.size());
            }

            // user-defined conversions to openCV's Input and Output Array types
            // this allows an Image instance to pretend to work like a cv::Mat
            operator cv::_InputArray() { return data->mat; }
            operator cv::_OutputArray() { return data->mat; }
            operator cv::Mat *() { return &(data->mat); }

        private:
            enum LockState_t
            {
                NONE,
                FROM_LABVIEW,
                FROM_CPP
            };
            struct ImagePersistantData
            {
                cv::Mat mat;
                LockState_t locked;
                std::mutex m;
                std::condition_variable cv;
            };

            // private functions required for initialization
            LV_EDVRContext_t get_ctx();
            LV_EDVRDataPtr_t create_new_edvr_data_ptr();
            LV_EDVRDataPtr_t get_edvr_data_ptr();
            ImagePersistantData *get_metadata();

            // locking and unlocking utility functions
            static inline void lock(ImagePersistantData *, LockState_t);
            static inline void unlock(ImagePersistantData *);

            // private properties - the order here is important for the initialization step
            const LV_EDVRReferencePtr_t edvr_ref_ptr;
            const LV_EDVRContext_t ctx;
            const LV_EDVRDataPtr_t edvr_data_ptr;
            ImagePersistantData *const data;
        };
    }
}
