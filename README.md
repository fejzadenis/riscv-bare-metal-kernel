# RISC-V Operating System Kernel

A small multithreaded OS kernel written from scratch for the RISC-V (RV64IMA)
architecture, running bare-metal under the QEMU emulator. It links against no standard
library and uses no services of the host operating system — threads, semaphores, memory
allocation and the system-call boundary between user and supervisor mode are all
implemented from the ground up.

## What it implements

- **Memory allocator** — first-fit continuous allocation over the raw physical heap,
  with block splitting and coalescing of adjacent free blocks.
- **Threads** — creation, termination and context switching, with the initial thread
  context hand-crafted so that a thread can begin executing for the first time.
- **Semaphores** — counting semaphores with FIFO wait queues, including multi-unit
  `wait_n` / `signal_n` operations.
- **System calls** — a single trap handler dispatching every `ecall`, exception and
  hardware interrupt, plus the three interface layers stacked above it.

## Architecture

The kernel and the application are linked into a single binary sharing one address
space, as in an embedded system. User code runs unprivileged and enters the kernel only
through `ecall`: arguments travel in registers `a0`–`a7`, the processor switches to
supervisor mode, the kernel serves the request and returns.

```
    ┌─────────────────────────────────┐
    │  C++ API                        │  Thread, Semaphore, Console
    ├─────────────────────────────────┤
    │  C API                          │  mem_alloc, thread_create, sem_wait, ...
    ├─────────────────────────────────┤
    │  ABI  (ecall)                   │  syscall code in a0, arguments in a1..a3, a7
    ╞═════════════════════════════════╡  <-- privilege boundary
    │  kernel                         │  _thread, _sem, Scheduler, MemoryAllocator
    └─────────────────────────────────┘
```

## Running

Requires the RISC-V GCC cross-toolchain and QEMU, along with the course-provided
`Makefile` and `lib/` directory (hardware stubs, not part of this repository).

```bash
make qemu
```

```bash
make qemu-gdb
```

The first target builds and runs the kernel; the second runs it under GDB.

## Scope

Memory allocation, threads and semaphores are complete across all three interface
layers. Time-sharing is not implemented — the timer interrupt is recognised and
acknowledged, but threads are never preempted, so they release the processor only by
yielding or blocking. `time_sleep` and `PeriodicThread` exist in the interface but
depend on that missing timer logic.

**Built with:** C++, RISC-V assembly, GCC cross-toolchain, QEMU, GDB, Make.

Coursework project for Operating Systems 1, School of Electrical Engineering,
University of Belgrade.
