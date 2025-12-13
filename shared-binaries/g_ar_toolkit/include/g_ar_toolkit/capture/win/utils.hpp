#pragma once

#include <stdexcept>
#include <Rpc.h>

struct GUIDHash
{
    std::size_t operator()(const GUID &g) const;
};

template <typename T>
struct CoTaskMemFreeDeleter
{
    void operator()(T *p) const
    {
        CoTaskMemFree(*p);
    }
};