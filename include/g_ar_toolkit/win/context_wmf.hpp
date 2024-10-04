
#ifdef _WIN32
#ifndef G_AR_TK__INTEROP_LV_CONTEXTWMF_HPP_
#define G_AR_TK__INTEROP_LV_CONTEXTWMF_HPP_

#include <stdexcept>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <future>

#include <windows.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <Rpc.h>
#include <mfreadwrite.h>

#include "../capture/context.hpp"

struct GUIDHash
{
    std::size_t operator()(const GUID &g) const
    {

        RPC_STATUS status;
        UUID uuid = g;
        auto hash = UuidHash(&uuid, &status);
        if (status != RPC_S_OK)
        {
            throw(std::out_of_range("Unable to generate hash of GUID"));
        }
        return hash;
    }
};

namespace g_ar_toolkit
{
    namespace capture
    {
        class ContextWMF : public g_ar_toolkit::capture::Context
        {
            
        public:
            ContextWMF();
            ~ContextWMF();
            void enumerate_devices(std::vector<device_info_t> &devices) override;
        private:
            enum class states
            {
                STARTING,
                WAITING_ON_ACTION,
                LISTING_DEVICES,
                STOPPING,
                STOPPED
            };
            enum class errors
            {
                NO_ERR,
                COM_INIT_ERR,
                MF_STARTUP_BAD_VER,
                MF_STARTUP_OTHER_ERR,
                DEVICE_ENUM_ERROR
            };
            states m_last_state;
            errors m_last_error;
            std::mutex m_mtx;
            const std::future<HRESULT> m_ftr;
            std::condition_variable m_notifier;
            std::vector<device_info_t> m_last_enumeration;
            void update_last_device_enumeration();
        };
    }

    template <typename T>
    struct CoTaskMemFreeDeleter
    {
        void operator()(T *p) const
        {
            CoTaskMemFree(*p);
        }
    };
}

#endif // G_AR_TK__INTEROP_LV_CONTEXTWMF_HPP_
#endif //_WIN32