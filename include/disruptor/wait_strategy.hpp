#pragma once

#include "disruptor/sequence.hpp"
#include <thread>

class wait_strategy
{
public:
    virtual int64_t wait_for( int64_t s, sequence& d ) = 0;
};

class yielding_wait_strategy : public wait_strategy
{
public:
    virtual int64_t wait_for( int64_t s, sequence& d );
};

inline int64_t yielding_wait_strategy::wait_for( int64_t seq, sequence& dep )
{
    int tries = 1000;
    int64_t avail_seq;

    while( tries-- && ( avail_seq = dep.get() ) < seq ) {
        std::this_thread::yield();
    }

    return avail_seq;
}
