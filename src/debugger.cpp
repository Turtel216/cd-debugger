#include "../include/debugger.h"
#include "../include/registers.h"

#include <fstream>
#include <iomanip>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <cstdint>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "dwarf/dwarf++.hh"
#include "linenoise.h"

auto split(const std::string &s, char delimiter) noexcept
    -> std::vector<std::string> {
  std::vector<std::string> out{};
  std::stringstream ss{s};
  std::string item;

  while (std::getline(ss, item, delimiter)) {
    out.push_back(item);
  }

  return out;
}

auto is_prefix(const std::string &s, const std::string &of) noexcept -> bool {
  if (s.size() > of.size())
    return false;
  return std::equal(s.begin(), s.end(), of.begin());
}

auto debugger::run() noexcept -> void {
  wait_for_signal();
  initialise_load_address();

  char *line = nullptr;
  while ((line = linenoise("cd-debugger> ")) != nullptr) {
    handle_command(line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
}

auto debugger::handle_command(const std::string &line) noexcept -> void {
  auto args = split(line, ' ');
  auto command = args[0];

  if (is_prefix(command, "continue")) {
    continue_execution();
  } else if (is_prefix(command, "break")) {
    std::string addr{args[1], 2};
    set_breakpoint_at_address(std::stol(addr, 0, 16));
  } else if (is_prefix(command, "register")) {
    if (is_prefix(args[1], "dump")) {
      dump_registers();
    } else if (is_prefix(args[1], "read")) {
      std::cout << get_register_value(m_pid, get_register_from_name(args[2]))
                << std::endl;
    } else if (is_prefix(args[1], "write")) {
      std::string val{args[3], 2}; // assume 0xVAL
      set_register_value(m_pid, get_register_from_name(args[2]),
                         std::stol(val, 0, 16));
    } else if (is_prefix(command, "memory")) {
      std::string addr{args[2], 2}; // assume 0xADDRESS

      if (is_prefix(args[1], "read")) {
        std::cout << std::hex << read_memory(std::stol(addr, 0, 16))
                  << std::endl;
      }
      if (is_prefix(args[1], "write")) {
        std::string val{args[3], 2}; // assume 0xVAL
        write_memory(std::stol(addr, 0, 16), std::stol(val, 0, 16));
      }
    } else {
      std::cerr << "Unknown command\n";
    }
  }
}

auto debugger::continue_execution() noexcept -> void {
  step_over_breakpoint();
  ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
  wait_for_signal();
}

auto debugger::set_breakpoint_at_address(std::intptr_t addr) noexcept -> void {
  std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;

  breakpoint bp{m_pid, addr};
  bp.enable();
  m_breakpoints[addr] = bp;
}

auto debugger::dump_registers() -> void {
  for (const auto &rd : g_register_descriptors) {
    std::cout << rd.name << " 0x" << std::setfill('0') << std::setw(16)
              << std::hex << get_register_value(m_pid, rd.r) << std::endl;
  }
}

auto debugger::get_pc() const noexcept -> std::uint64_t {
  return get_register_value(m_pid, reg::rip);
}

auto debugger::set_pc(std::uint64_t pc) -> void {
  set_register_value(m_pid, reg::rip, pc);
}

auto debugger::step_over_breakpoint() -> void {
  // - 1 because execution will go past the breakpoint
  auto possible_breakpoint_location = get_pc() - 1;

  if (m_breakpoints.count(possible_breakpoint_location)) {
    auto &bp = m_breakpoints[possible_breakpoint_location];

    if (bp.is_enabled()) {
      auto previous_instruction_address = possible_breakpoint_location;
      set_pc(previous_instruction_address);

      bp.disable();
      ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
      wait_for_signal();
      bp.enable();
    }
  }
}

auto debugger::wait_for_signal() const noexcept -> void {
  int wait_status;
  auto options = 0;
  waitpid(m_pid, &wait_status, options);
}

auto debugger::initialise_load_address() noexcept -> void {
  // If this is a dynamic library (e.g. PIE)
  if (m_elf.get_hdr().type == elf::et::dyn) {
    std::ifstream map("/proc/" + std::to_string(m_pid) + "/maps");

    // Read the first address from the file
    std::string addr;
    std::getline(map, addr, '-');

    m_load_address = std::stoi(addr, 0, 16);
  }
}

auto debugger::read_memory(std::uint64_t address) const noexcept
    -> std::uint64_t {
  return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);
}

auto debugger::write_memory(std::uint16_t address,
                            std::uint64_t value) const noexcept -> void {
  ptrace(PTRACE_POKEDATA, m_pid, address, value);
}

auto debugger::get_function_from_pc(std::uint64_t pc) -> dwarf::die {
  for (auto &cu : m_dwarf.compilation_units()) {
    if (dwarf::die_pc_range(cu.root()).contains(pc)) {
      for (const auto &die : cu.root()) {
        if (die.tag == dwarf::DW_TAG::subprogram) {
          if (dwarf::die_pc_range(die).contains(pc)) {
            return die;
          }
        }
      }
    }
  }
  throw std::out_of_range("Cannot find function");
}

auto debugger::get_line_entry_from_pc(std::uint64_t pc)
    -> dwarf::line_table::iterator {
  for (auto &cu : m_dwarf.compilation_units()) {
    if (die_pc_range(cu.root()).contains(pc)) {
      auto &lt = cu.get_line_table();
      auto it = lt.find_address(pc);
      if (it == lt.end()) {
        throw std::out_of_range{"Cannot find line entry"};
      } else {
        return it;
      }
    }
  }

  throw std::out_of_range{"Cannot find line entry"};
}

auto debugger::offset_load_address(std::uint64_t addr) -> std::uint64_t {
  return addr - m_load_address;
}

auto debugger::print_source(const std::string &file_name, unsigned line,
                            unsigned n_lines_context) -> void {
  std::ifstream file{file_name};

  // Work out a window around the desired line
  auto start_line = line <= n_lines_context ? 1 : line - n_lines_context;
  auto end_line = line + n_lines_context +
                  (line < n_lines_context ? n_lines_context - line : 0) + 1;

  char c{};
  auto current_line = 1u;
  // Skip lines up until start_line
  while (current_line != start_line && file.get(c)) {
    if (c == '\n') {
      ++current_line;
    }
  }

  // Output cursor if we're at the current line
  std::cout << (current_line == line ? "> " : "  ");

  // Write lines up until end_line
  while (current_line <= end_line && file.get(c)) {
    std::cout << c;
    if (c == '\n') {
      ++current_line;
      // Output cursor if we're at the current line
      std::cout << (current_line == line ? "> " : "  ");
    }
  }

  // Write newline and make sure that the stream is flushed properly
  std::cout << std::endl;
}
