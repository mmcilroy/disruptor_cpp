#pragma once

#include "disruptor/sequence.hpp"
#include <vector>
#include <thread>

class sequencer;

class sequence_barrier {
public:
    sequence_barrier( sequencer&, sequence& );
    sequence_barrier( sequencer&, sequence&, std::initializer_list< sequence* > );

    int64_t wait_for( int64_t );

private:
    int64_t min_dependent_seq();

    sequence _cursor_seq;
    sequencer& _sequencer;
    std::vector< sequence* > _dependent_seq;
};

inline sequence_barrier::sequence_barrier( sequencer& s, sequence& d ) : 
    _sequencer( s )
{
    _dependent_seq.push_back( &d );
}

inline sequence_barrier::sequence_barrier( sequencer& s, sequence& d, std::initializer_list< sequence* > l ) :
    _sequencer( s ),
    _dependent_seq( l )
{
}

inline int64_t sequence_barrier::wait_for( int64_t seq )
{
    long tries = 1000;
    long avail_seq;

    while( tries-- && ( avail_seq = min_dependent_seq() ) < seq ) {
        std::this_thread::yield();
    }

    return avail_seq;
}

inline int64_t sequence_barrier::min_dependent_seq()
{
    int64_t min = std::numeric_limits< int64_t >::max();
    for( sequence* s : _dependent_seq ) {
        min = std::min( s->get(), min );
    }

    return min;
}
