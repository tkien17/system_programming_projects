# 🌐 TCP Programming Client

## Overview
This project implements a robust networked client in C that establishes a connection to a remote server over a TCP socket. After authenticating, the client enters an active listening state to receive dynamic mathematical tasks, computes the bitwise result locally, and transmits the answer back. The loop continues continuously until the client successfully captures the flag ("BYE" message).

## Key Features
* **Reliable Socket Initialization:** Connects to remote hosts using `AF_INET` and `SOCK_STREAM`.
* **Stream Data Buffering:** Network streams do not guarantee complete messages in a single packet. This client manages an internal `memmove` buffer to safely extract complete, newline-delimited command strings without dropping concurrent data.
* **Bitwise Manipulations:** Capable of actively parsing formatted string commands (`LEFT`, `RIGHT`, `OR`, `AND`) and performing the corresponding bit-shifting and hex-masking operations.

## Usage
Run the compiled executable with three arguments: your student email, the target port, and the target IPv4 address.
```bash
./tcp_programming <knguyenquang@umass.edu> <port> <host_ip>