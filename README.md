disruptor_cpp
===========

A simplistic C++ implementation of the LMAX disruptor (https://github.com/LMAX-Exchange/disruptor)

```
g++ -O3 -std=c++11 -Wl,--no-as-needed -I ../include one_to_one_sequenced_throughput_test.cpp -lpthread
```

one_to_one_sequenced_throughput_test performance:

java

```
Run 0, Disruptor=49,825,610 ops/sec
Run 1, Disruptor=48,496,605 ops/sec
Run 2, Disruptor=85,251,491 ops/sec
Run 3, Disruptor=83,682,008 ops/sec
Run 4, Disruptor=85,910,652 ops/sec
Run 5, Disruptor=88,967,971 ops/sec
Run 6, Disruptor=92,506,938 ops/sec
```

c++

```
Run 0, Disruptor=159,744,408 ops/sec
Run 1, Disruptor=163,934,426 ops/sec
Run 2, Disruptor=164,203,612 ops/sec
Run 3, Disruptor=163,666,121 ops/sec
Run 4, Disruptor=165,016,501 ops/sec
Run 5, Disruptor=163,132,137 ops/sec
Run 6, Disruptor=163,934,426 ops/sec
```
