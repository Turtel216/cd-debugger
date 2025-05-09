# Crazy-Diamond

**Crazy-Diamond** is a lightweight, modular debugger implemented in C++ designed for low-level control and introspection of program execution. It supports essential debugging functionality such as breakpoints, stepping, register and memory manipulation, and backtrace inspection.

## ✨ Features

* **Program Control**

  * Launch new processes
  * Halt and continue execution
* **Breakpoints**

  * Set breakpoints on:

    * Specific memory addresses
    * Source code lines
    * Function entry points
* **Stepping**

  * Single instruction step
  * Step into functions
  * Step out of current function
  * Step over function calls
* **Inspection**

  * Read and write CPU registers
  * Read and write memory
  * Print current source location
  * Show backtrace of current execution stack
  * Print values of simple variables

## 🧱 Project Structure

```
Crazy-Diamond/
├── include/            # Public header files
├── src/                # Core implementation of the debugger
├── examples/           # Sample programs for testing/debugging
├── external/           # Third-party libraries (git submodules)
│   ├── libelfin/       # DWARF and ELF parsing
│   └── linenoise/      # Minimalist command-line editing
├── CMakeLists.txt      # Build configuration
└── README.md           # You are here
```

## 🚀 Getting Started

### Prerequisites

* C++17 compatible compiler
* CMake ≥ 3.12
* POSIX-compatible OS (Linux recommended)

### Build Instructions

```bash
git clone --recurse-submodules https://github.com/Turtel216/cd-debugger.git
cd cd-debugger
cmake -B build -S .
cmake --build build
```

### Running the Debugger

Use the debugger on an example binary:

```bash
./cdb ../examples/hello_world
```

## 🧪 Examples

The `examples/` directory contains simple C++ programs compiled with debug symbols to test Crazy-Diamond’s features. Use these to explore stepping, breakpoints, and memory/register inspection.

## 📂 Dependencies

Crazy-Diamond includes the following open-source libraries as submodules:

* [libelfin](https://github.com/aclements/libelfin) – for DWARF and ELF parsing
* [linenoise](https://github.com/antirez/linenoise) – for command-line editing and history

## 💡 Future Plans

* Watchpoints and conditional breakpoints
* Scripting interface
* UI enhancements
* Remote debugging support

## 📄 License

MIT License. See [LICENSE](./LICENSE) for details.
