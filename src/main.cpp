#include "disruptor/sequencer.hpp"
#include "disruptor/ring_buffer.hpp"
#include "disruptor/batch_event_processor.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace disruptor;

const long long N = 1000 * 1000 * 1000;

template<class T>
batch_event_processor<T>::batch_event_processor(ring_buffer<T>& r, sequence& p, sequence& s) :
    _rb(r), _pub(p), _sub(s) {
}

template<class T>
void batch_event_processor<T>::run() {
    long next_seq = _sub.get() + 1;
    long avail_seq;
    for (long i = 0; i < N;) {
        while ((avail_seq = _pub.get()) < next_seq) {
            std::this_thread::yield();
        }
        while (next_seq <= avail_seq) {
            //std::cout << "subscriber: " << rb[next_seq] << ", expecting: " << i << std::endl;
            assert(_rb[next_seq] == i);
            ++i;
            ++next_seq;
        }
        _sub.set(avail_seq);
    }
}

int main() {
    for(long j=0; j<8; j++) {
        ring_buffer<long> rb(1024 * 64);
        sequence ps, ss;
        sequencer seq(ps, ss, rb.size());
        batch_event_processor<long> ep(rb, ps, ss);

        std::thread t(&batch_event_processor<long>::run, &ep);
        boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();

        for (long i = 0; i < N; i++) {
            long next = seq.next();
            rb[next] = i;
            seq.publish();
        }

        t.join();
        boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration diff = t2 - t1;
        long long rate = (N * 1000) / (diff.total_milliseconds());
        std::cout << "complete in: " << diff.total_milliseconds() << " ms, rate " << rate << std::endl;
    }
}
