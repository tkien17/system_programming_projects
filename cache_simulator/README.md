# ⚙️ Cache Simulator

## Overview
This application is a low-level CPU cache simulator written in C, designed to model detailed memory access behavior and evaluate caching performance metrics. By ingesting realistic memory trace files, it mimics how a localized CPU cache layer responds to varying programmatic workloads.

## Key Features
* **Configurable Parameters:** Allows deep configuration of cache architecture upon launch, accepting specific set bits, associativity sizes (lines), and block bit parameters.
* **Trace Processing:** Parses external trace files line-by-line to execute a virtual stream of memory accesses.
* **Modular Architecture:** Functionality is separated cleanly between the virtual CPU module (`cpu.c`) and the core cache mechanisms (`cache.c`).

## Usage
The executable strictly requires four command-line parameters to define the hardware constraints and dictate the input file:
```bash
./cache_simulator <set bits> <associativity> <block bits> <tracefile>
```

## Testing and Execution Output

The simulator is thoroughly tested using the GoogleTest framework to verify cache initialization, bitwise indexing, and internal state management. Below is an execution trace from the EdLab environment demonstrating a successful test suite run, followed by an actual simulation mapping the `wc.trace` file using 2 set bits, 2 lines of associativity, and 10 block bits.

<details>
<summary>Click to expand test and simulation output</summary>
knguyenquang@elnux1:~/cs230/project_3$ ./cache_test
Running main() from ./googletest/src/gtest_main.cc
[==========] Running 8 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 8 tests from ProjectTests
[ RUN      ] ProjectTests.test_cache_make
[       OK ] ProjectTests.test_cache_make (0 ms)
[ RUN      ] ProjectTests.test_cache_initialization_set_bits
[       OK ] ProjectTests.test_cache_initialization_set_bits (0 ms)
[ RUN      ] ProjectTests.test_cache_initialization_block_bits
[       OK ] ProjectTests.test_cache_initialization_block_bits (0 ms)
[ RUN      ] ProjectTests.test_cache_initialization_line_count
[       OK ] ProjectTests.test_cache_initialization_line_count (0 ms)
[ RUN      ] ProjectTests.test_cache_set_and_block_counts
[       OK ] ProjectTests.test_cache_set_and_block_counts (0 ms)
[ RUN      ] ProjectTests.test_cache_set_lists
[       OK ] ProjectTests.test_cache_set_lists (0 ms)
[ RUN      ] ProjectTests.test_cache_set_lists_line_count
[       OK ] ProjectTests.test_cache_set_lists_line_count (0 ms)
[ RUN      ] ProjectTests.test_wc_trace
hits: 787666 misses: 22104 evictions: 22088 hrate: 0.972703 mrate: 0.027297
[       OK ] ProjectTests.test_wc_trace (240 ms)
[----------] 8 tests from ProjectTests (242 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 1 test suite ran. (242 ms total)
[  PASSED  ] 8 tests.

knguyenquang@elnux1:~/cs230/project_3$ ./cache_app 6 8 6 test/wc.trace
hits: 805029 misses: 4741 evictions: 4229 hrate: 0.994145 mrate: 0.005855
```text