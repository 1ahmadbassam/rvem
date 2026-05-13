# RVEM

**RVEM** is a lightweight, portable RISC-V emulator written in C99. It is designed to simulate RV32 and RV64 architectures, providing a bare-metal environment for testing and running RISC-V binaries. 

## Features

* **Configurable Architecture:** Support for both **RV32I** and **RV64I** base instruction sets.
* **Standard Extensions:** Support for the **M (Integer Multiplication and Division)** extension.
* **Performance Modeling:** Basic cycle-accurate performance tracking, including stalls for data hazards, branch penalties, and memory access latency.
* **Bare-Metal SDK:** Includes a custom Board Support Package (BSP) with a linker script (`link.ld`), startup code (`crt0.S`), and a lightweight C library (`librv`) featuring a minimal `printf` implementation.
* **CSR Support:** Implements essential Control and Status Registers (e.g., `mcycle`, `minstret`) for performance benchmarking.
* **Portable:** Built with strict C99 standard compliance. Compiles on GCC, Clang, and MSVC.

## Repository Structure

```text
.
├── CMakeLists.txt    # CMake configuration for building the host emulator
├── Makefile          # Makefile for building guest tests and the SDK
├── doc/              # RISC-V ISA manuals and documentation
├── inc/              # Emulator header files (arch, cpu, bus, riscv, etc.)
├── sdk/              # Bare-metal Software Development Kit
│   ├── bsp/          # Board Support Package (crt0.S, link.ld)
│   └── librv/        # Lightweight C library (printf, performance counters)
├── src/              # Emulator source code
│   ├── cpu/          # Core CPU logic and CSR implementation
│   └── main.c        # Emulator entry point and ROM loading
└── tests/            # Test programs (fibonacci, im_stress, etc.)

```

## Memory Map

The emulator defines a simple memory map (configurable in `inc/arch.h` and `sdk/bsp/link.ld`):

* **ROM (Instruction Memory):** `0x00000000` (Size: 512 KB)
* **RAM (Data Memory):** `0x20000000` (Size: 64 KB)

## Building the Emulator (Host)

RVEM uses CMake to build the host emulator. By default, it builds an optimized release binary for the RV32IM architecture.

### Prerequisites

* CMake (3.10 or higher)
* A C99 compliant compiler (GCC, Clang, or MSVC)

### Build Instructions

```bash
# 1. Create a build directory
mkdir build && cd build

# 2. Configure the project
# Optional flags: 
#   -DRV64=ON  (Build for RV64 instead of RV32)
#   -DM_EXT=ON (Enable the 'M' extension, default is ON)
cmake .. -DCMAKE_BUILD_TYPE=Release 

# 3. Build the emulator
cmake --build .

```

The compiled executable will be located at `build/bin/rvem` (or `build/bin/rvem.exe` on Windows).

## Building and Running Tests (Guest)

The project includes a `Makefile` at the root directory that orchestrates building the RISC-V guest binaries using the RISC-V GNU Compiler Toolchain and running them on the emulator.

### Prerequisites

* `riscv64-unknown-elf-gcc` (or `riscv32-unknown-elf-gcc`) available in your PATH.

### Running a Test

You can build and execute a test program in one go using the root `Makefile`. You must specify the test name.

```bash
# Build the SDK and the fibonacci test, then run it
make emulate TEST=fibonacci

```

**Customizing the Test Build:**
You can match the guest toolchain architecture to your emulator build by passing `RV64` and `M_EXT` variables to `make`:

```bash
# Build and run a test for RV64 with the M extension disabled
make emulate TEST=multiplication RV64=1 M_EXT=0

```

### Manual Execution

If you have an existing flat binary (`.bin`) compiled for the emulator's memory map, you can run it directly by passing it as an argument to the emulator:

```bash
./build/bin/rvem path/to/your_program.bin

```

## SDK and `librv`

The `sdk/` directory contains everything needed to write C programs for RVEM:

* **`librv.h`**: Provides inline assembly macros to read hardware performance counters (`query_performance_counter()`, `query_instruction_counter()`).
* **`printf.h`**: A minimal, dependency-free `printf` implementation that routes output via a custom `ecall` (handled by the emulator in `core.c`).
* **`crt0.S`**: Initializes the global pointer, stack pointer, copies initialized data from ROM to RAM, zeroes out the BSS section, and calls `main()`. Gracefully halts the emulator via `ecall` with `a0=10` upon exit.