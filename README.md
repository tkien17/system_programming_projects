# 🧠 System Programming Projects

## Overview
This repository showcases a collection of low-level systems programming projects written in C, focused on building core components of operating systems and network communication. Each project demonstrates a different facet of Unix/Linux system-level programming, emphasizing mastery of system calls, process control, signals, network sockets, and resource management. 

## 🧩 Repository Structure

* `cache_simulator/` # Cache memory performance simulator 
* `simulated_parallel_processing/` # Multi-core processing simulation using pipes and signals 
* `tcp_programming/` # TCP client for dynamic network communication and bitwise computation

## 🧠 Projects Summary

### ⚙️ Cache Simulator
**Goal:** Build a simulator to model CPU cache behavior and analyze memory access efficiency.
**Highlights:**
* Implements configurable cache allocation logic handling varying set bits, associativity, and block bits.
* Processes extensive trace files to accurately simulate read/write interactions within a constrained cache architecture.
* **🧰 Core Skills:** Memory management, data structure allocation, cache architectures, and performance simulation.

### 🚀 Simulated Parallel Processing
**Goal:** Simulate multi-core parallel processing using processes, pipes, and signals.
**Highlights:**
* Main process coordinates three child processes acting as CPU cores.
* Two-way communication via pipes, and result notification via real-time signals (`SIGRTMAX`).
* Uses atomic signal handling (`volatile sig_atomic_t`) for safe, non-blocking communication.
* Demonstrates real-time scheduling, task distribution, and concurrent result aggregation.
* **🧰 Core Skills:** Process synchronization, signal handling, IPC, concurrent task scheduling.

### 🌐 TCP Programming
**Goal:** Implement a TCP client that communicates with a remote server to solve dynamic computational tasks in a Capture The Flag (CTF) style challenge.
**Highlights:**
* Establishes network communication using standard TCP sockets (`socket()`, `connect()`, `inet_pton()`).
* Implements dynamic stream buffering to safely read partial messages and extract complete newline-delimited tasks.
* Parses server instructions to execute localized bitwise manipulations (`LEFT`, `RIGHT`, `OR`, `AND`) and returns the computed results to capture the flag.

## 🧰 Technical Stack
* **Languages:** C 
* **Core APIs:** `fork()`, `wait()`, `pipe()`, `sigaction()`, `kill()`, `socket()`, `recv()`, `send()`

**Author:** Kien Nguyen
**Focus Areas:** OS Internals, Concurrency, Networking, IPC