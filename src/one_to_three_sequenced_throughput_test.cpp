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

#include "disruptor/ring_buffer.hpp"
#include "disruptor/batch_event_processor.hpp"
#include "test_helpers.hpp"

const int64_t ITERATIONS = 1000L * 1000L * 100L;
const size_t BUFFER_SIZE = 1024 * 8;
const size_t NUM_PROCESSORS = 3;

int64_t run_pass()
{
    test_event_handler handler[ NUM_PROCESSORS ];
    std::unique_ptr< wait_strategy > wait( new yielding_wait_strategy() );
    std::unique_ptr< ring_buffer< test_event > > ring = std::move( ring_buffer< test_event >::create_single_producer( BUFFER_SIZE, *wait ) );
    std::unique_ptr< sequence_barrier > barrier = std::move( ring->make_barrier() );
    std::unique_ptr< batch_event_processor< test_event > > processor[] =  {
        std::unique_ptr< batch_event_processor< test_event > >( new batch_event_processor< test_event >( *ring, *barrier, handler[0] ) ),
        std::unique_ptr< batch_event_processor< test_event > >( new batch_event_processor< test_event >( *ring, *barrier, handler[1] ) ),
        std::unique_ptr< batch_event_processor< test_event > >( new batch_event_processor< test_event >( *ring, *barrier, handler[2] ) )
    };

    ring->add_gating_sequences( {
        &processor[0]->get_sequence(),
        &processor[1]->get_sequence(),
        &processor[2]->get_sequence()
    } );

    for( long i=0; i<NUM_PROCESSORS; i++ ) {
        handler[i].reset( ITERATIONS );
    }

    std::thread processor_thread0( &batch_event_processor< test_event >::run, processor[0].get() );
    std::thread processor_thread1( &batch_event_processor< test_event >::run, processor[1].get() );
    std::thread processor_thread2( &batch_event_processor< test_event >::run, processor[2].get() );
    test_time t1 = time_now();

    for( int64_t i=0; i<ITERATIONS; i++ )
    {
        int64_t next = ring->next();
        ring->get( next )._data = i;
        ring->publish( next );
    }

    for( long i=0; i<NUM_PROCESSORS; i++ )
    {
        handler[i].await();
        processor[i]->halt();
    }

    processor_thread0.join();
    processor_thread1.join();
    processor_thread2.join();

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
