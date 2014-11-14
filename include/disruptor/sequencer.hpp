#pragma once

#include "disruptor/sequence_barrier.hpp"
#include <thread>

class sequencer
{
public:
    sequencer( size_t n, wait_strategy& w );

    std::unique_ptr< sequence_barrier > make_barrier();
    std::unique_ptr< sequence_barrier > make_barrier( std::initializer_list< sequence* > );

    void add_gating_sequences( std::initializer_list< sequence* > );

    size_t get_buffer_size();
    int64_t next( size_t n=1 );
    void publish( int64_t );

private:
    int64_t min_gating_seq( int64_t next );

    int _buffer_size;
    int64_t _curr;
    int64_t _cache;
    sequence _cursor;
    std::vector< sequence* > _gating_seq;
    wait_strategy& _wait;
};

inline sequencer::sequencer( size_t n, wait_strategy& w ) :
    _buffer_size( n ),
    _curr( -1 ),
    _cache( -1 ),
    _wait( w )
{
}

inline std::unique_ptr< sequence_barrier > sequencer::make_barrier()
{
    return std::unique_ptr< sequence_barrier >( new sequence_barrier( *this, _cursor, _wait ) );
}

inline std::unique_ptr< sequence_barrier > sequencer::make_barrier( std::initializer_list< sequence* > l )
{
    return std::unique_ptr< sequence_barrier >( new sequence_barrier( *this, l, _wait ) );
}

inline void sequencer::add_gating_sequences( std::initializer_list< sequence* > s )
{
    _gating_seq = s;
}

inline size_t sequencer::get_buffer_size()
{
    return _buffer_size;
}

inline int64_t sequencer::next( size_t n )
{
    int64_t next = _curr + 1;
    int64_t wrap = next - _buffer_size;

    if( wrap > _cache || _cache > next )
    {
        while( wrap > ( _cache = min_gating_seq( next ) ) ) {
            std::this_thread::yield();
        }
    }

    return ( _curr = next );
}

inline void sequencer::publish( int64_t s )
{
    _cursor.set( s );
}

inline int64_t sequencer::min_gating_seq( int64_t next )
{
    int64_t min = next;

    for( sequence* s : _gating_seq ) {
        min= std::min( s->get(), min );
    }

    return min;
}
