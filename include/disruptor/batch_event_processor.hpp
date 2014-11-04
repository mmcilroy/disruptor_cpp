#pragma once

#include "sequence.hpp"
#include "ring_buffer.hpp"
#include <thread>

namespace disruptor {

template<class T>
class batch_event_processor {
public:
    batch_event_processor(ring_buffer<T>& r, sequence& p, sequence& s);
    void run();

private:
    ring_buffer<T>& _rb;
    sequence& _pub;
    sequence& _sub;
};

}
