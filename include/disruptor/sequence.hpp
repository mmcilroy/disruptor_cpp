#pragma once

#include <atomic>

class sequence
{
public:
    sequence();
    void set( int64_t );
    int64_t get() const;

private:
    int64_t _val;
};

inline sequence::sequence()
{
    set( -1 );
}

inline long sequence::get() const
{
    std::atomic_thread_fence( std::memory_order::memory_order_acquire );
    return _val;
}

inline void sequence::set( long n )
{
    std::atomic_thread_fence( std::memory_order::memory_order_release );
    _val = n;
}
