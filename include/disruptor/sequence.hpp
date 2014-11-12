#pragma once

#include <atomic>

class sequence {
public:
    typedef long value_type;
    sequence();
    value_type get() const;
    void set(value_type);

private:
    char _pad0[28];
    value_type _val;
    char _pad1[28];
};

inline sequence::sequence() {
    set(-1);
}

inline long sequence::get() const {
    std::atomic_thread_fence(std::memory_order::memory_order_acquire);
    return _val;
}

inline void sequence::set(long n) {
    std::atomic_thread_fence(std::memory_order::memory_order_release);
    _val = n;
}
