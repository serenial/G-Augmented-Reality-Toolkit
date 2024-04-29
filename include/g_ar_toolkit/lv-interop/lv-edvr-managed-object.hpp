// an object that works similar to image::Image but
// only uses the EDVR for lifetime managment and never
// sets the sub-array

#ifndef G_AR_TK__INTEROP_LV_EDVR_MNGD_OBJ_HPP_
#define G_AR_TK__INTEROP_LV_EDVR_MNGD_OBJ_HPP_

#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>

#include "./lv-types.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {

        // an which can interact with EDVR references and the persistant data that they refer to
        // similar to Image but only uses the EDVR for lifetime management and never assigns
        // any memory to the EDVR-sub-array
        template <typename T>
        class EDVRManagedObject
        {
        private:
            struct persistant_data
            {
                T* object;
                bool locked;
                std::mutex m;
                std::condition_variable cv;
                const std::function<LV_MgErr_t(T*,LV_EDVRDataPtr_t)> on_lock, on_unlock;
                persistant_data(std::function<LV_MgErr_t(T*, LV_EDVRDataPtr_t)> on_lock, std::function<LV_MgErr_t(T*, LV_EDVRDataPtr_t ptr)> on_unlock): on_lock(on_lock), on_unlock(on_unlock){}
                ~persistant_data(){
                    delete object;
                }
            };

            // locking and unlocking utility functions
            static inline void lock(EDVRManagedObject::persistant_data *d)
            {
                // obtain the mutex
                std::unique_lock lk(d->m);
                // wait for the locked flag to be false
                d->cv.wait(lk, [&]
                           { return !(d->locked); });
                // set the locked flag
                d->locked = true;
                // unlock and notify
                lk.unlock();
                d->cv.notify_one();
            }
            static inline void unlock(EDVRManagedObject::persistant_data *d)
            {
                {
                    // obtain the mutex
                    std::lock_guard lk(d->m);
                    // clear the locked flag
                    d->locked = false;
                }
                // scoped-unlock and notify
                d->cv.notify_one();
            }

            // private functions required for initialization
            LV_EDVRContext_t get_ctx()
            {
                LV_EDVRContext_t ctx;
                auto err = EDVR_GetCurrentContext(&ctx);
                if (err)
                {
                    throw std::runtime_error("Failed to obtain the application-context.");
                }
                return ctx;
            }

            LV_EDVRDataPtr_t create_new_edvr_data_ptr(std::function<LV_MgErr_t(T*,LV_EDVRDataPtr_t)> on_lock, std::function<LV_MgErr_t(T*,LV_EDVRDataPtr_t)> on_unlock)
            {
                // initialize EDVR reference and get new EDVR data_ptr

                LV_EDVRDataPtr_t data_ptr = nullptr;
                auto err = EDVR_CreateReference(edvr_ref_ptr, &data_ptr);
                if (err)
                {
                    throw std::runtime_error("Unable to create a data-reference to associate with the supplied EDVR Refnum.");
                }

                // initialize Imagepersistant_data object
                persistant_data *persistant_data_ptr = new persistant_data(on_lock, on_unlock);

                // set the metadata_ptr of the new edvr data pointer
                data_ptr->metadata_ptr = reinterpret_cast<uintptr_t>(persistant_data_ptr);

                // locking and unlocking from labVIEW side
                data_ptr->lock_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr){
                    persistant_data *pdp = reinterpret_cast<persistant_data *>(ptr->metadata_ptr);
                    return pdp->on_lock(pdp->object, ptr);
                };

                data_ptr->unlock_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr){
                    persistant_data *pdp = reinterpret_cast<persistant_data *>(ptr->metadata_ptr);
                    return pdp->on_unlock(pdp->object, ptr);
                };

                data_ptr->delete_callback_fn_ptr = [](LV_EDVRDataPtr_t ptr)
                {
                    persistant_data *pdp = reinterpret_cast<persistant_data *>(ptr->metadata_ptr);
                    // obtain the mutex
                    std::unique_lock lk(pdp->m);
                    // wait for the locked flag to be cleared
                    pdp->cv.wait(lk, [&]
                                  { return !(pdp->locked); });
                    // free the lock
                    lk.unlock();
                    // delete persistant_data
                    delete pdp;
                    ptr->metadata_ptr = reinterpret_cast<uintptr_t>(nullptr);
                };

                return data_ptr;
            }

            LV_EDVRDataPtr_t get_edvr_data_ptr()
            {
                LV_EDVRDataPtr_t data_ptr = nullptr;
                auto err = EDVR_AddRefWithContext(*edvr_ref_ptr, ctx, &data_ptr);
                if (err)
                {
                    throw std::runtime_error("Unable to dereference the supplied EDVR Refnum to valid data in this application-context.");
                }
                return data_ptr;
            }

            persistant_data *get_data()
            {
                return reinterpret_cast<persistant_data *>(edvr_data_ptr->metadata_ptr);
            }

            // private properties - the order here is important for the initialization step
            const LV_EDVRReferencePtr_t edvr_ref_ptr;
            const LV_EDVRContext_t ctx;
            const LV_EDVRDataPtr_t edvr_data_ptr;
            persistant_data *const data;

        public:
            // create from exisiting object
            EDVRManagedObject(LV_EDVRReferencePtr_t edvr_ref_ptr) : edvr_ref_ptr(edvr_ref_ptr), ctx(get_ctx()),
                                                                    edvr_data_ptr(get_edvr_data_ptr()),
                                                                    data(get_data())
            {
                lock(data);
            }

            // initialize and add new object
            EDVRManagedObject(
                LV_EDVRReferencePtr_t edvr_ref_ptr, 
                T* object, 
                std::function<void(T* ,const LV_EDVRDataPtr_t)> edvr_value_set_function = [](auto ptr, auto e_data_ptr)
                                                                                          {
                        // set the sub-array to be a 1-D empty array
                        e_data_ptr->n_dims = 1;
                        e_data_ptr->sub_array.dimension_specifier[0] = {0, 1}; },
                std::function<LV_MgErr_t(T*, LV_EDVRDataPtr_t)> on_lock = [](auto p, auto d){ return LV_ERR_noError;},
                std::function<LV_MgErr_t(T*, LV_EDVRDataPtr_t)> on_unlock = [](auto p, auto d){ return LV_ERR_noError;}
            ) : edvr_ref_ptr(edvr_ref_ptr), ctx(0), edvr_data_ptr(create_new_edvr_data_ptr(on_lock,on_unlock)), data(get_data())
            {
                data->object = object;
                edvr_value_set_function(data->object, edvr_data_ptr);
                data->locked = true; // exclusive access so can lock manually
            }

            ~EDVRManagedObject()
            {
                unlock(data);
                // release ref if originally added
                if (ctx)
                {
                    EDVR_ReleaseRefWithContext(*edvr_ref_ptr, ctx);
                }
            }

            T* get_object()
            {
                return data->object;
            }
        };
    }
}

#endif //G_AR_TK__INTEROP_LV_EDVR_MNGD_OBJ_HPP_