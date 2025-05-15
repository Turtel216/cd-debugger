/**
 * @file debugger.h
 * @brief Defines the core debugger class for program debugging capabilities.
 *
 * This file contains the debugger class which provides functionality for
 * debugging programs, including breakpoint management, memory inspection,
 * register manipulation, source code examination, and execution control.
 * It utilizes DWARF and ELF information for runtime debugging information.
 */

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include "breakpoint.h"
#include "dwarf/dwarf++.hh"
#include "elf/elf++.hh"
#include <fcntl.h>
#include <linux/types.h>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <utility>

/**
 * @class debugger
 * @brief Main debugger class that controls the debugging of a program.
 *
 * The debugger class provides functionality for program debugging, including:
 * - Setting and managing breakpoints
 * - Controlling program execution (continue, step, etc.)
 * - Inspecting program memory and registers
 * - Querying source code information via DWARF debug data
 * - Command processing for user interaction
 */
class debugger {
public:
  /**
   * @brief Constructs a debugger for a specific program.
   *
   * Initializes the debugger and loads ELF and DWARF information from the
   * specified program for use during debugging.
   *
   * @param prog_name Name/path of the program to debug
   * @param pid Process ID of the program being debugged
   */
  debugger(std::string prog_name, pid_t pid) noexcept
      : m_prog_name{std::move(prog_name)}, m_pid{pid} {
    auto fd = open(m_prog_name.c_str(), O_RDONLY);
    m_elf = elf::elf{elf::create_mmap_loader(fd)};
    m_dwarf = dwarf::dwarf{dwarf::elf::create_loader(m_elf)};
  };

  /**
   * @brief Starts the debugger's main loop.
   *
   * This function runs the debugger's main command loop, processing
   * user commands and controlling the debugging session.
   */
  auto run() noexcept -> void;

  /**
   * @brief Sets a breakpoint at the specified memory address.
   *
   * @param addr The memory address where the breakpoint should be placed
   */
  auto set_breakpoint_at_address(std::intptr_t addr) noexcept -> void;

private:
  const std::string m_prog_name; ///< Name/path of the program being debugged
  pid_t m_pid;                   ///< Process ID of the program being debugged
  std::unordered_map<std::intptr_t, breakpoint>
      m_breakpoints;            ///< Map of active breakpoints
  dwarf::dwarf m_dwarf;         ///< DWARF debug information for the program
  elf::elf m_elf;               ///< ELF information for the program
  std::uint64_t m_load_address; ///< Base load address of the program in memory

  /**
   * @brief Processes a command entered by the user.
   *
   * @param line The command string to process
   */
  auto handle_command(const std::string &line) noexcept -> void;

  /**
   * @brief Continues execution of the debugged program.
   *
   * Resumes execution of the program until the next breakpoint
   * or program termination.
   */
  auto continue_execution() noexcept -> void;

  /**
   * @brief Displays the values of CPU registers.
   *
   * Outputs the current values of relevant CPU registers
   * from the debugged program.
   */
  auto dump_registers() -> void;

  /**
   * @brief Reads a 64-bit value from the debugged program's memory.
   *
   * @param address The memory address to read from
   * @return The 64-bit value at the specified memory address
   */
  auto read_memory(std::uint64_t address) const noexcept -> std::uint64_t;

  /**
   * @brief Writes a 64-bit value to the debugged program's memory.
   *
   * @param address The memory address to write to
   * @param value The 64-bit value to write
   */
  auto write_memory(std::uint16_t address, std::uint64_t value) const noexcept
      -> void;

  /**
   * @brief Gets the current program counter (PC) value.
   *
   * @return The current value of the program counter register
   */
  auto get_pc() const noexcept -> std::uint64_t;

  /**
   * @brief Sets the program counter (PC) to a specific value.
   *
   * @param pc The value to set the program counter to
   */
  auto set_pc(std::uint64_t pc) -> void;

  /**
   * @brief Handles execution when the program counter is at a breakpoint.
   *
   * This function manages the execution of a single instruction when
   * the debugged program is stopped at a breakpoint.
   */
  auto step_over_breakpoint() -> void;

  /**
   * @brief Waits for a signal from the debugged program.
   *
   * This function blocks until the debugged program sends a signal,
   * such as when it hits a breakpoint or terminates.
   */
  auto wait_for_signal() const noexcept -> void;

  /**
   * @brief Initializes the load address of the debugged program.
   *
   * Determines and stores the base address where the program
   * has been loaded into memory.
   */
  auto initialise_load_address() noexcept -> void;

  /**
   * @brief Gets the function containing a specific program counter value.
   *
   * @param pc The program counter value to look up
   * @return The DWARF debugging information for the function
   */
  auto get_function_from_pc(std::uint64_t pc) -> dwarf::die;

  /**
   * @brief Gets the source line information for a specific program counter
   * value.
   *
   * @param pc The program counter value to look up
   * @return An iterator to the corresponding entry in the DWARF line table
   */
  auto get_line_entry_from_pc(std::uint64_t pc) -> dwarf::line_table::iterator;

  /**
   * @brief Adjusts an address by the program's load address.
   *
   * @param addr The address to adjust
   * @return The adjusted address relative to the program's load address
   */
  auto offset_load_address(std::uint64_t addr) -> std::uint64_t;

  /**
   * @brief Displays source code around a specific line in a file.
   *
   * @param file_name The source file name
   * @param line The line number to center the display around
   * @param n_lines_context The number of context lines to show before and after
   */
  auto print_source(const std::string &file_name, unsigned line,
                    unsigned n_lines_context) -> void;
};

#endif // DEBUGGER_H_
