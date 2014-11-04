#pragma once

#include <atomic>

namespace disruptor {

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

#include "sequence.inl"

}
