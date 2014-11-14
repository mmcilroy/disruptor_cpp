disruptor_cpp
=============

A C++ implementation of the LMAX disruptor (https://github.com/LMAX-Exchange/disruptor
). Goals are as follows...

. Provide similar or better performance characteristics
. Provide a similar interface to the java implementation

The current implementation supports only single producers

In its current state performance tests show rougly a x2 performance improvement over the java implementation as demonstrated by the output from the one_to_one_sequenced_throughput_test

java

```
Run 0, Disruptor=49,212,598 ops/sec
Run 1, Disruptor=51,921,079 ops/sec
Run 2, Disruptor=50,864,699 ops/sec
Run 3, Disruptor=59,630,292 ops/sec
Run 4, Disruptor=62,227,753 ops/sec
Run 5, Disruptor=59,988,002 ops/sec
Run 6, Disruptor=60,569,351 ops/sec
```

c++

```
Run 0, Disruptor=95,877,277 ops/sec
Run 1, Disruptor=94,786,729 ops/sec
Run 2, Disruptor=93,632,958 ops/sec
Run 3, Disruptor=92,506,938 ops/sec
Run 4, Disruptor=95,057,034 ops/sec
Run 5, Disruptor=93,632,958 ops/sec
Run 6, Disruptor=94,876,660 ops/sec
```

Example compile command:
```
g++ -O3 -std=c++11 -Wl,--no-as-needed -I ../include one_to_one_sequenced_throughput_test.cpp -lpthread
```
