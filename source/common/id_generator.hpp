#pragma once

#include <atomic>

namespace common
{
    class IDGenerator
    {
    public:
        static uint64_t next()
        {
            return s_nextId.fetch_add( 1, std::memory_order_acq_rel );
        }

    private:
        static inline std::atomic< uint64_t > s_nextId { 0 };
    };
}