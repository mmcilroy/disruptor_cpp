/**
 * UniCast a series of items between 1 publisher and 1 event processor.
 *
 * +----+    +-----+
 * | P1 |--->| EP1 |
 * +----+    +-----+
 *
 * Disruptor:
 * ==========
 *              track to prevent wrap
 *              +------------------+
 *              |                  |
 *              |                  v
 * +----+    +====+    +====+   +-----+
 * | P1 |--->| RB |<---| SB |   | EP1 |
 * +----+    +====+    +====+   +-----+
 *      claim      get    ^        |
 *                        |        |
 *                        +--------+
 *                          waitFor
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 */

#include <boost/date_time/posix_time/posix_time.hpp>
#include "disruptor/ring_buffer.hpp"
#include "disruptor/batch_event_processor.hpp"
#include "abstract_perf_test.hpp"
#include "test_event_handler.hpp"

const long ITERATIONS = 1000L * 1000L * 100L;
const size_t BUFFER_SIZE = 1024 * 64;

class one_to_one_sequenced_throughput_test : public abstract_perf_test {
public:
    one_to_one_sequenced_throughput_test();
    ~one_to_one_sequenced_throughput_test();

protected:
    virtual long run_pass();

private:
    ring_buffer<test_event>* _ring;
    batch_event_processor<test_event>* _processor;
    sequence_barrier* _barrier;
    test_event_handler _handler;
};

one_to_one_sequenced_throughput_test::one_to_one_sequenced_throughput_test() {
    run();
}

one_to_one_sequenced_throughput_test::~one_to_one_sequenced_throughput_test() {
    ;
}

long one_to_one_sequenced_throughput_test::run_pass() {
    _ring = ring_buffer<test_event>::create_single_producer(BUFFER_SIZE);
    _barrier = _ring->make_barrier();
    _processor = new batch_event_processor<test_event>(*_ring, *_barrier, _handler);
    _ring->add_gating_sequences({&_processor->get_sequence()});

    _handler.reset(ITERATIONS);
    std::thread processor_thread(&batch_event_processor<test_event>::run, _processor);
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
    for(long i=0; i<ITERATIONS; i++) {
        long next = _ring->next();
        _ring->get(next)._data = i;
        _ring->publish(next);
    }
    _handler.await();
    _processor->halt();
    processor_thread.join();

    boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = t2 - t1;
    return (ITERATIONS * 1000) / (diff.total_milliseconds());
}

int main() {
    one_to_one_sequenced_throughput_test test;
}
