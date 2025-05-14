#include "../include/debugger.h"
#include "../include/registers.h"

#include <iomanip>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <cstdint>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
  int wait_status;
  auto options = 0;
  waitpid(m_pid, &wait_status, options);

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

auto debugger::read_memory(std::uint64_t address) const noexcept
    -> std::uint64_t {
  return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);
}

auto debugger::write_memory(std::uint16_t address,
                            std::uint64_t value) const noexcept
    -> std::uint64_t {
  ptrace(PTRACE_POKEDATA, m_pid, address, value);
}
