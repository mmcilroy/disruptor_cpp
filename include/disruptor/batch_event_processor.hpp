#pragma once

#include "disruptor/ring_buffer.hpp"
#include "disruptor/event_handler.hpp"
#include <thread>

template< class T >
class batch_event_processor
{
public:
    batch_event_processor( ring_buffer<T>&, sequence_barrier&, event_handler<T>& );

    sequence& get_sequence();
    void run();
    void halt();

private:
    ring_buffer<T>& _ring;
    event_handler<T>& _handler;
    sequence_barrier& _barrier;
    sequence _seq;
    bool _run;
};

template< class T >
inline batch_event_processor<T>::batch_event_processor( ring_buffer<T>& r, sequence_barrier& b, event_handler<T>& h ) :
    _ring( r ),
    _barrier( b ),
    _handler( h ),
    _run( true )
{
}

template< class T >
sequence& batch_event_processor<T>::get_sequence()
{
    return _seq;
}

template< class T >
void batch_event_processor<T>::run()
{
    int64_t next_seq = _seq.get() + 1;
    while( _run )
    {
        long avail_seq = _barrier.wait_for( next_seq );
        while( next_seq <= avail_seq )
        {
            _handler.on_event( _ring.get( next_seq ), next_seq, next_seq == avail_seq );
            ++next_seq;
        }
        _seq.set( avail_seq );
    }
}

template< class T >
void batch_event_processor<T>::halt()
{
    _run = false;
}
