#pragma once

#include "disruptor/event_handler.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <thread>

typedef boost::posix_time::ptime test_time;

test_time time_now()
{
    return boost::posix_time::microsec_clock::local_time();
}

struct test_event
{
    int64_t _data;
};

class test_event_handler : public event_handler< test_event >
{
public:
    void reset( size_t n );
    void await();

    virtual void on_event( const test_event&, int64_t, bool );

private:
    size_t _count;
    size_t _curr;
};

inline void test_event_handler::reset( size_t n )
{
    _count = n;
    _curr = 0;
}

inline void test_event_handler::await()
{
    while( _count ) {
        std::this_thread::yield();
    }
}

inline void test_event_handler::on_event( const test_event& e, int64_t s, bool b )
{
    assert( _curr == e._data );
    _curr++;
    _count--;
}
