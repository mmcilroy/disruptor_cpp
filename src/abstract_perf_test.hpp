#pragma once

class abstract_perf_test {
public:
    virtual ~abstract_perf_test();

protected:
    void run();
    virtual long run_pass() = 0;
};



#include <iostream>

inline abstract_perf_test::~abstract_perf_test() {
    ;
}

inline void abstract_perf_test::run() {
    static const int RUNS = 7;
    std::cout.imbue(std::locale(""));
    std::cout << "Starting Disruptor tests" << std::endl;
    for(int i=0; i<RUNS; i++) {
        long ops = run_pass();
        std::cout << "Run " << i << ", Disruptor=" << std::fixed << ops << " ops/sec" << std::endl;
    }
}
