# HTTP Client/Server with Priority Packet Scheduling



A multithreaded HTTP/1.1 client/server implementation built from scratch in C using POSIX sockets, with a custom priority-based packet scheduler that controls transmission order based on packet priority labels (`H` = high, `L` = low).

---

## Overview

This project has two main components:

1. **HTTP Client/Server** — handles HTTP/1.1 GET/POST requests, response formatting, and error handling over raw sockets (no networking libraries)
2. **Packet Scheduler** — sits between the client and server, reads a packet count and priority string (e.g. `5 HLHLH`), and reorders outgoing packets so all high-priority packets transmit before low-priority ones

The scheduler simulates how a router prioritizes traffic — a useful model for understanding QoS (Quality of Service) in real networks.

---

## How the scheduler works

Given input `5 HLHLH`:

```
Incoming:   pkt0(H)  pkt1(L)  pkt2(H)  pkt3(L)  pkt4(H)
                         |
                  [ Scheduler ]
                  /           \
         High queue          Low queue
         [0, 2, 4]             [1, 3]
                  \           /
                   Drain H first
                         |
Output order:   0  2  4  1  3
```

High-priority packets are always drained first, preserving relative arrival order within each priority class.

---

## Compilation

**Using make (recommended):**
```bash
make
```

**Manual:**
```bash
gcc packet_scheduling.c -o packet_scheduling
```

---

## Usage

```bash
./packet_scheduling < input.in
```

Input format: `<num_packets> <priority_string>`

```
5 HLHLH
```

Output: space-separated packet indices in transmission order

```
0 2 1 4 3
```

---

## Test cases

### Case 1 — correct input
```bash
echo "5 HLHLH" | ./packet_scheduling
# Expected: 0 2 1 4 3
```

### Case 2 — mismatched length
```bash
echo "4 HLL" | ./packet_scheduling
# Expected: Wrong input: the number of packets is wrong.
```

### Case 3 — zero packets
```bash
echo "0 HLHLH" | ./packet_scheduling
# Expected: Wrong input: the number of packets must be greater than 0.
```

### Case 4 — invalid priority character
```bash
echo "4 HXHH" | ./packet_scheduling
# Expected: Wrong input: the priority must be H or L.
```

---

## Input validation

The program catches and reports:
- Packet count of 0 or negative
- Priority string length not matching declared packet count
- Priority characters other than `H` or `L`

---

## Technologies

- **C** — core implementation
- **POSIX sockets** — raw TCP for HTTP client/server
- **HTTP/1.1** — request/response parsing, status codes, error handling
- **Wireshark** — used for traffic inspection and debugging during development

---

## Repository structure

```
/src
  packet_scheduling.c   # Scheduler + HTTP client/server
  Makefile
/tests
  sample1.in            # Valid input
  sample2.in            # Wrong priority string length
  sample3.in            # Zero packets
  sample4.in            # Invalid priority character
```

---

## Author

**Vivek Reddy Kasireddy** — [LinkedIn](https://linkedin.com) | [Website](https://vivek.com)  
WPI Computer Science & Robotics Engineering, Class of 2026
