# KhaosOS

**Interactive operating-system simulator** in C++: time-sliced CPU scheduling, multiprogramming with variable partition memory, and I/O to printers, disks, and CD-ROM–style devices. Built as an academic OS course project; useful as a portfolio piece to discuss concurrency concepts, resource allocation, and device handling in interviews.

## What it demonstrates

- **Process model**: PCBs, ready queue, job pool, process creation (`A`), termination (`t`), and quantum expiry (`T`).
- **CPU scheduling**: Configurable quantum (milliseconds); dispatcher pulls the next process when the CPU is idle.
- **Memory management**: Word-addressable memory, first-fit style allocation with holes, compaction when needed, and snapshot of physical layout (`s` / `S` → `m`).
- **I/O and interrupts**: System calls (`p`/`d`/`c` + device index) vs device completion interrupts (`P`/`D`/`C`); per-device queues.
- **Disk-oriented behavior**: Cylinder tracking and disk-specific scheduling hooks (e.g. NHDSA-related logic for disk operations).

## Requirements

- A C++ compiler with C++11 or later (GCC, Clang, or MSVC).

## Build

From the project root:

```bash
g++ -std=c++11 -O2 -o khaos MP_Khaos_Main_Test.cpp
```

**Windows (MSVC, Developer Command Prompt):**

```cmd
cl /EHsc /std:c++14 MP_Khaos_Main_Test.cpp /Fe:khaos.exe
```

## Run

```bash
./khaos
```

On Windows: `khaos.exe`

The program prompts for **quantum length** (ms), **memory size** (words), and counts for **printers**, **disks**, and **CD-ROM** devices (0–9 each). Then enter single- or two-character commands at the `User Command:` prompt. Use `h` or `H` for the in-app command list, `q` or `Q` to exit.

## Project layout

| File | Role |
|------|------|
| `MP_Khaos_Main_Test.cpp` | Entry point: configuration prompts, starts init and run loop |
| `MP_Khaos_OpSys.cpp` | System init/run, command dispatch, snapshot UI |
| `MP_Khaos_Functions.h` | Memory, CPU, device list, and related implementations |
| `MP_Khaos_Structs_Classes.h` | `PCB`, `Memory`, `DevList`, `CPU` |
| `MP_Khaos_Global.h` | Shared globals |
Original implementation: Michael Patterson (course project; instructor Eric Schweitzer). Repository files retain in-source authorship notes.
