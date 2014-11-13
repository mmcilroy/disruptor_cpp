#pragma once

#include "disruptor/sequencer.hpp"
#include <memory>

template< class T >
class ring_buffer
{
public:
    static std::unique_ptr< ring_buffer<T> > create_single_producer( size_t n );

    std::unique_ptr< sequence_barrier > make_barrier();
    std::unique_ptr< sequence_barrier > make_barrier( std::initializer_list< sequence* > );

    void add_gating_sequences( std::initializer_list< sequence* > );

    int64_t next();
    T& get( int64_t );
    void publish( int64_t );

private:
    ring_buffer( std::unique_ptr< sequencer > );

    size_t _mask;
    std::unique_ptr< T[] > _buffer;
    std::unique_ptr< sequencer > _sequencer;
};

template< class T >
inline std::unique_ptr< ring_buffer<T> > ring_buffer<T>::create_single_producer( size_t n )
{
    return std::unique_ptr< ring_buffer<T> >( new ring_buffer<T>( 
        std::unique_ptr< sequencer >( new sequencer( n ) ) ) );
}

template< class T >
inline ring_buffer<T>::ring_buffer( std::unique_ptr< sequencer > s ) :
    _mask( s->get_buffer_size() - 1 ),
    _buffer( new T[ s->get_buffer_size() ] ),
    _sequencer( std::move( s ) )
{
    // throw if size is not ^2
}

template< class T >
inline std::unique_ptr< sequence_barrier > ring_buffer<T>::make_barrier()
{
    return _sequencer->make_barrier();
}

template< class T >
inline std::unique_ptr< sequence_barrier > ring_buffer<T>::make_barrier( std::initializer_list< sequence* > l )
{
    return _sequencer->make_barrier( l );
}

template< class T >
inline void ring_buffer<T>::add_gating_sequences( std::initializer_list< sequence* > l )
{
    _sequencer->add_gating_sequences( l );
}

template< class T >
int64_t ring_buffer<T>::next()
{
    return _sequencer->next();
}

template< class T >
T& ring_buffer<T>::get( int64_t i )
{
    return _buffer[ i & _mask ];
}

template< class T >
void ring_buffer<T>::publish( int64_t i )
{
    _sequencer->publish( i );
}
