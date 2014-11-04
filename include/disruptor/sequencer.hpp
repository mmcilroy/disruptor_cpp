#pragma once

#include "disruptor/sequence.hpp"
#include <thread>

namespace disruptor {

class sequencer {
public:
    sequencer(sequence& p, sequence& s, size_t n);
    sequence::value_type next(size_t n = 1);
    void publish();

private:
    sequence& _pub_seq;
    sequence& _sub_seq;
    size_t _size;
    long _curr;
    long _cache;
};

#include "sequencer.inl"

}
