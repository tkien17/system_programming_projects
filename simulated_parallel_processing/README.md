# 🚀 Simulated Parallel Processing

## Overview
This system program simulates concurrent parallel processing across three dedicated CPU core subprocesses. The primary parent process functions as a dynamic scheduler: it decomposes a large job into distinct independent tasks, farms them out to idle processing cores, continuously aggregates the results, and finalizes a structural summary.

## Inter-Process Communication (IPC)
This simulation relies on robust, non-blocking IPC coordination:
* **Bidirectional Pipes:** Each child core is initialized with two dedicated pipelines (`main_to_core` and `core_to_main`) to establish two-way communication.
* **Real-time Signals:** Each core utilizes a specific, unique real-time signal (`SIGRTMAX`, `SIGRTMAX-1`, `SIGRTMAX-2`) to instantly alert the parent when a computation finishes. Real-time signals are specifically leveraged to prevent signal drops when multiple cores resolve their tasks simultaneously.
* **Atomic Handlers & Concurrency:** Instead of blocking via `read()` and stalling other cores, the parent scheduler continuously loops. It only attempts to read a pipe if an atomic `volatile sig_atomic_t` counter indicates data is queued, maximizing throughput.

## Execution Flow
1.  **Creation:** Three child handlers are initiated via `fork()`. Extraneous pipe ends are forcefully closed to guarantee memory safety and EOF signaling.
2.  **Task Dispatch:** The parent maps integers into string payloads (e.g., `task_id_n`) and executes a standard `write()` into the core's pipe.
3.  **Core Execution:** The core parses the payload, computes the bitwise result, triggers a `kill()` signal to the parent, and flushes the data to its return pipe.
4.  **Graceful Termination:** Following task exhaustion, all primary pipes are shut down. `wait(NULL)` ensures zombie processes are reaped.

## Usage
The executable requires the total number of tasks and a bit extraction limit to drive the scheduling loop.
```bash
./proc_comm_app <total_tasks> <max_bits>
```

## Sample Execution Output

Below is a live execution trace run on the EdLab environment. It demonstrates the asynchronous, non-blocking nature of the main process as it distributes 8 tasks across 3 cores and processes out-of-order completions via real-time signals.

<details>
<summary>Click to expand terminal output</summary>
```text
Total number of tasks: 8
Maximum number of bits for each task: 3
[LOG] Main process: Created Core 1 subprocess (PID 851294).
[LOG] Main process: Created Core 2 subprocess (PID 851295).
[LOG] Main process: Created Core 1 subprocess (PID 0).
[LOG] Core 1 (PID 851294): Closed main_to_core[0] write end and unused pipe ends.
[LOG] Main process: Created Core 3 subprocess (PID 851296).
[LOG] Main process: Closed unused pipe ends.
[LOG] Main process: Assigning task 0 to Core 1.
[LOG] Main process: Assigning task 1 to Core 2.
[LOG] Main process: Created Core 2 subprocess (PID 0).
[LOG] Main process: Assigning task 2 to Core 3.
[LOG] Core 2 (PID 851295): Closed main_to_core[1] write end and unused pipe ends.
[LOG] Core (PID 851294): Received task 0 with n=2.
[LOG] Core (PID 851295): Received task 1 with n=1.
[LOG] Main process: Created Core 3 subprocess (PID 0).
[LOG] Core 3 (PID 851296): Closed main_to_core[2] write end and unused pipe ends.
[LOG] Core (PID 851296): Received task 2 with n=2.
[LOG] Core (PID 851294): Completed task 0, result=0. Sending to main.
[LOG] Core (PID 851295): Completed task 1, result=1. Sending to main.
[LOG] Core (PID 851294): Sending signal to main process.
[LOG] Core (PID 851295): Sending signal to main process.
[LOG] Main process: Received result 0 from Core 1.
[LOG] Main process: Received result 1 from Core 2.
[LOG] Main process: Assigning task 3 to Core 1.
[LOG] Core (PID 851296): Completed task 2, result=2. Sending to main.
[LOG] Main process: Assigning task 4 to Core 2.
[LOG] Core (PID 851294): Received task 3 with n=3.
[LOG] Core (PID 851296): Sending signal to main process.
[LOG] Core (PID 851295): Received task 4 with n=1.
[LOG] Main process: Received result 2 from Core 3.
[LOG] Main process: Assigning task 5 to Core 3.
[LOG] Core (PID 851296): Received task 5 with n=3.
[LOG] Core (PID 851294): Completed task 3, result=3. Sending to main.
[LOG] Core (PID 851294): Sending signal to main process.
[LOG] Main process: Received result 3 from Core 1.
[LOG] Main process: Assigning task 6 to Core 1.
[LOG] Core (PID 851295): Completed task 4, result=0. Sending to main.
[LOG] Core (PID 851295): Sending signal to main process.
[LOG] Main process: Received result 0 from Core 2.
[LOG] Core (PID 851294): Received task 6 with n=2.
[LOG] Main process: Assigning task 7 to Core 2.
[LOG] Core (PID 851296): Completed task 5, result=5. Sending to main.
[LOG] Core (PID 851296): Sending signal to main process.
[LOG] Main process: Received result 5 from Core 3.
[LOG] Core (PID 851295): Received task 7 with n=1.
[LOG] Core (PID 851294): Completed task 6, result=2. Sending to main.
[LOG] Core (PID 851294): Sending signal to main process.
[LOG] Main process: Received result 2 from Core 1.
[LOG] Core (PID 851295): Completed task 7, result=1. Sending to main.
[LOG] Core (PID 851295): Sending signal to main process.
[LOG] Main process: Received result 1 from Core 2.
[LOG] Main process: All 8 tasks completed. Closing pipes and waiting for children.
[LOG] Core 1 (PID 851294): Finished all tasks, exiting.
[LOG] Core 2 (PID 851295): Finished all tasks, exiting.
[LOG] Core 3 (PID 851296): Finished all tasks, exiting.
[LOG] Main process: All child processes have beene reaped.
Results:
Core 1: 0, 3, 2
Core 2: 1, 0, 1
Core 3: 2, 5