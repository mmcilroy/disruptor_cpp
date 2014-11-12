#pragma once

#include "disruptor/sequence_barrier.hpp"
#include "disruptor/sequencer.hpp"
#include <cstddef>

template<class T>
class ring_buffer {
public:
    static ring_buffer<T>* create_single_producer(size_t n);

    ring_buffer(sequencer&);
    ~ring_buffer();

    sequence_barrier* make_barrier();
    sequence_barrier* make_barrier(std::vector<sequence*> s);
    void add_gating_sequences(std::vector<sequence*> s);

    sequence::value_type next();
    T& get(sequence::value_type);
    void publish(sequence::value_type);

private:
    T* _buffer;
    sequencer& _sequencer;
    size_t _mask;
};

template<class T>
ring_buffer<T>* ring_buffer<T>::create_single_producer(size_t n) {
    return new ring_buffer<T>(*new sequencer(n));
}

template<class T>
inline ring_buffer<T>::ring_buffer(sequencer& s) :
    _buffer(new T[s.get_buffer_size()]),
    _sequencer(s),
    _mask(s.get_buffer_size()-1) {
}

template<class T>
inline ring_buffer<T>::~ring_buffer() {
    delete[] _buffer;
}

template<class T>
inline sequence_barrier* ring_buffer<T>::make_barrier() {
    return _sequencer.make_barrier();
}

template<class T>
inline sequence_barrier* ring_buffer<T>::make_barrier(std::vector<sequence*> s) {
    return _sequencer.make_barrier(s);
}

template<class T>
inline void ring_buffer<T>::add_gating_sequences(std::vector<sequence*> s) {
    _sequencer.add_gating_sequences(s);
}

template<class T>
sequence::value_type ring_buffer<T>::next() {
    return _sequencer.next();
}

template<class T>
T& ring_buffer<T>::get(sequence::value_type i) {
    return _buffer[i&_mask];
}

template<class T>
void ring_buffer<T>::publish(sequence::value_type s) {
    _sequencer.publish(s);
}
