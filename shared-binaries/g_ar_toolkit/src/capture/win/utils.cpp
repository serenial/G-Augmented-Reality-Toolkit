#include "g_ar_toolkit/capture/win/utils.hpp"

std::size_t GUIDHash::operator()(const GUID &g) const
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