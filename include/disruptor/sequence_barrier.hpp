#pragma once

#include "disruptor/sequence.hpp"
#include "disruptor/wait_strategy.hpp"
#include <vector>
#include <thread>

class sequencer;

class sequence_barrier {
public:
    sequence_barrier( sequencer&, sequence&, wait_strategy& );
    sequence_barrier( sequencer&, std::initializer_list< sequence* >, wait_strategy& );

    int64_t wait_for( int64_t );

private:
    sequence& min_dependent_seq();

    sequence _cursor_seq;
    sequencer& _sequencer;
    wait_strategy& _wait;
    std::vector< sequence* > _dependent_seq;
};

inline sequence_barrier::sequence_barrier( sequencer& s, sequence& d, wait_strategy& w ) : 
    _sequencer( s ),
    _wait( w )
{
    _dependent_seq.push_back( &d );
}

inline sequence_barrier::sequence_barrier( sequencer& s, std::initializer_list< sequence* > l, wait_strategy& w ) :
    _sequencer( s ),
    _wait( w ),
    _dependent_seq( l )
{
}

inline int64_t sequence_barrier::wait_for( int64_t seq )
{
    int64_t avail_seq = _wait.wait_for( seq, min_dependent_seq() );
    return avail_seq;
}

inline sequence& sequence_barrier::min_dependent_seq()
{
    sequence* seq;
    int64_t min = std::numeric_limits< int64_t >::max();
    for( sequence* s : _dependent_seq )
    {
        int64_t i = s->get();
        if( i < min )
        {
            min = i;
            seq = s;
        }
    }
    return *seq;
}
