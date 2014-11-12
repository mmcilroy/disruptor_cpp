/**
 * MultiCast a series of items between 1 publisher and 3 event processors.
 *
 *           +-----+
 *    +----->| EP1 |
 *    |      +-----+
 *    |
 * +----+    +-----+
 * | P1 |--->| EP2 |
 * +----+    +-----+
 *    |
 *    |      +-----+
 *    +----->| EP3 |
 *           +-----+
 *
 * Disruptor:
 * ==========
 *                             track to prevent wrap
 *             +--------------------+----------+----------+
 *             |                    |          |          |
 *             |                    v          v          v
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 * | P1 |--->| RB |<---| SB |    | EP1 |    | EP2 |    | EP3 |
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 *      claim      get    ^         |          |          |
 *                        |         |          |          |
 *                        +---------+----------+----------+
 *                                      waitFor
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 * EP2 - EventProcessor 2
 * EP3 - EventProcessor 3
 */

#include <boost/date_time/posix_time/posix_time.hpp>
#include "disruptor/ring_buffer.hpp"
#include "disruptor/batch_event_processor.hpp"
#include "abstract_perf_test.hpp"
#include "test_event_handler.hpp"

const long ITERATIONS = 1000L * 1000L * 100L;
const size_t BUFFER_SIZE = 1024 * 64;
const size_t NUM_PROCESSORS = 3;

class one_to_three_sequenced_throughput_test : public abstract_perf_test {
public:
    one_to_three_sequenced_throughput_test();
    ~one_to_three_sequenced_throughput_test();

protected:
    virtual long run_pass();

private:
    ring_buffer<test_event>* _ring;
    sequence_barrier* _barrier;
    batch_event_processor<test_event>* _processor[NUM_PROCESSORS];
    test_event_handler _handler[NUM_PROCESSORS];
};

one_to_three_sequenced_throughput_test::one_to_three_sequenced_throughput_test() {
    run();
}

one_to_three_sequenced_throughput_test::~one_to_three_sequenced_throughput_test() {
    ;
}

long one_to_three_sequenced_throughput_test::run_pass() {
    _ring = ring_buffer<test_event>::create_single_producer(BUFFER_SIZE);
    _barrier = _ring->make_barrier();
    for(int i=0; i<NUM_PROCESSORS; i++) {
        _processor[i] = new batch_event_processor<test_event>(*_ring, *_barrier, _handler[i]);
    }
    _ring->add_gating_sequences({&_processor[0]->get_sequence(), &_processor[1]->get_sequence(), &_processor[2]->get_sequence()});
    for(int i=0; i<NUM_PROCESSORS; i++) {
        _handler[i].reset(ITERATIONS);
    }

    std::thread processor_thread0(&batch_event_processor<test_event>::run, _processor[0]);
    std::thread processor_thread1(&batch_event_processor<test_event>::run, _processor[1]);
    std::thread processor_thread2(&batch_event_processor<test_event>::run, _processor[2]);

    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
    for(long i=0; i<ITERATIONS; i++) {
        long next = _ring->next();
        _ring->get(next)._data = i;
        _ring->publish(next);
        //std::cout << "published " << i << std::endl;
    }

    for(int i=0; i<NUM_PROCESSORS; i++) {
        _handler[i].await();
    }
    for(int i=0; i<NUM_PROCESSORS; i++) {
        _processor[i]->halt();
    }

    processor_thread0.join();
    processor_thread1.join();
    processor_thread2.join();

    boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = t2 - t1;
    return (ITERATIONS * 1000) / (diff.total_milliseconds());
}

int main() {
    one_to_three_sequenced_throughput_test test;
}
