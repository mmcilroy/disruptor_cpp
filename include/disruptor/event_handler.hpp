#pragma once

#include "disruptor/sequence.hpp"

template<class T>
class event_handler {
public:
    virtual void on_event(const T&, sequence::value_type, bool) = 0;
};
