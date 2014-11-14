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

#include "disruptor/ring_buffer.hpp"
#include "disruptor/batch_event_processor.hpp"
#include "test_helpers.hpp"

const int64_t ITERATIONS = 1000L * 1000L * 100L;
const size_t BUFFER_SIZE = 1024 * 64;

int64_t run_pass()
{
    test_event_handler handler;
    std::unique_ptr< wait_strategy > wait( new yielding_wait_strategy() );
    std::unique_ptr< ring_buffer< test_event > > ring = std::move( ring_buffer< test_event >::create_single_producer( BUFFER_SIZE, *wait ) );
    std::unique_ptr< sequence_barrier > barrier = std::move( ring->make_barrier() );
    std::unique_ptr< batch_event_processor< test_event > > processor( new batch_event_processor< test_event >( *ring, *barrier, handler ) );
    ring->add_gating_sequences( { &processor->get_sequence() } );
    handler.reset( ITERATIONS );

    std::thread processor_thread( &batch_event_processor< test_event >::run, processor.get() );
    test_time t1 = time_now();

    for( int64_t i=0; i<ITERATIONS; i++ )
    {
        int64_t next = ring->next();
        ring->get( next )._data = i;
        ring->publish( next );
    }

    handler.await();
    processor->halt();
    processor_thread.join();

    return ( ITERATIONS * 1000 ) / ( ( time_now() - t1 ).total_milliseconds() );
}

int main()
{
    static const int RUNS = 7;
    std::cout.imbue( std::locale( "" ) );
    for( int i=0; i<RUNS; i++ )
    {
        long ops = run_pass();
        std::cout << "Run " << i << ", Disruptor=" << std::fixed << ops << " ops/sec" << std::endl;
    }
}
