#pragma once

#include "disruptor/sequence.hpp"

template< class T >
class event_handler
{
public:
    virtual void on_event( const T&, int64_t, bool ) = 0;
};
