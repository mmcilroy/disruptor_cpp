#pragma once

#include "disruptor/event_handler.hpp"
#include "test_event.hpp"
#include <thread>
#include <cassert>

class test_event_handler : public event_handler<test_event> {
public:
    void reset(long n);
    void await();

    virtual void on_event(const test_event&, sequence::value_type, bool);

private:
    long _count;
    long _curr;
};

inline void test_event_handler::reset(long n) {
    _count = n;
    _curr = 0;
}

inline void test_event_handler::await() {
    while(_count) {
        std::this_thread::yield();
    }
}

inline void test_event_handler::on_event(const test_event& e, sequence::value_type s, bool b) {
    //std::cout << "received " << e._data << std::endl;
    assert(_curr == e._data);
    --_count;
    ++_curr;
}
