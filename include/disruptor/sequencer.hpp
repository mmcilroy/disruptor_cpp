#pragma once

#include "disruptor/sequence_barrier.hpp"
#include "disruptor/sequence.hpp"
#include <thread>

class sequencer {
public:
    sequencer(size_t n);

    sequence_barrier* make_barrier();
    sequence_barrier* make_barrier(std::vector<sequence*>);
    void add_gating_sequences(std::vector<sequence*>);
    size_t get_buffer_size();
    sequence::value_type next(size_t n=1);
    void publish(sequence::value_type);

private:
    sequence::value_type min_gating_seq(long next);

    int _buffer_size;
    sequence::value_type _curr;
    sequence::value_type _cache;
    sequence _cursor;
    std::vector<sequence*> _gating_seq;
};

inline sequencer::sequencer(size_t n) :
    _buffer_size(n),
    _curr(-1),
    _cache(-1),
    _gating_seq(0) {
}

inline sequence_barrier* sequencer::make_barrier() {
    return new sequence_barrier(*this, _cursor);
}

inline sequence_barrier* sequencer::make_barrier(std::vector<sequence*> s) {
    return 0; //new sequence_barrier(*this, _cursor, s);
}

inline void sequencer::add_gating_sequences(std::vector<sequence*> s) {
    _gating_seq = s;
}

inline size_t sequencer::get_buffer_size() {
    return _buffer_size;
}

inline sequence::value_type sequencer::next(size_t n) {
    long next = _curr + 1;
    long wrap = next - _buffer_size;
    if(wrap > _cache || _cache > next) {
        while (wrap > (_cache = min_gating_seq(next))) {
            std::this_thread::yield();
        }
    }
    _curr = next;
    return _curr;
}

inline void sequencer::publish(sequence::value_type s) {
    _cursor.set(s);
}

inline sequence::value_type sequencer::min_gating_seq(long next) {
    sequence::value_type m = next;
    for(sequence* s:_gating_seq) {
        m = std::min(s->get(),m);
    }
    return m;
}
