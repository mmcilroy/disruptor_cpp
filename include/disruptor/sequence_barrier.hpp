#pragma once

#include "disruptor/sequence.hpp"
#include <vector>
#include <thread>

class sequencer;

class sequence_barrier {
public:
    sequence_barrier(sequencer&, sequence&);
    sequence_barrier(sequencer&, sequence&, std::vector<sequence*>);
    sequence::value_type wait_for(sequence::value_type);

private:
    sequence::value_type min_dependent_seq();

    sequence _cursor_seq;
    sequencer& _sequencer;
    std::vector<sequence*> _dependent_seq;
};

inline sequence_barrier::sequence_barrier(sequencer& s, sequence& d) : 
    _sequencer(s) {
    _dependent_seq.push_back(&d);
}

inline sequence_barrier::sequence_barrier(sequencer& s, sequence& d, std::vector<sequence*> v) :
    _sequencer(s),
    _dependent_seq(v) {
}

inline sequence::value_type sequence_barrier::wait_for(sequence::value_type seq) {
    long tries = 1000;
    long avail_seq;
    while(tries-- && (avail_seq = min_dependent_seq()) < seq) {
        std::this_thread::yield();
    }
    return avail_seq;
}

inline sequence::value_type sequence_barrier::min_dependent_seq() {
    sequence::value_type m = std::numeric_limits<sequence::value_type>::max();
    for(sequence* s:_dependent_seq) {
        m = std::min(s->get(),m);
    }
    return m;
}
