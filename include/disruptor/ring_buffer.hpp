#pragma once

namespace disruptor {

template<class T>
class ring_buffer {
public:
    ring_buffer(size_t n);
    ~ring_buffer();
    size_t size();
    T& operator[](int i);

private:
    T* _buffer;
    size_t _size;
    size_t _mask;
};

#include "ring_buffer.inl"

}
