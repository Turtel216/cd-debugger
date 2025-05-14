#include "../include/debugger.h"

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
  } else {
    std::cerr << "Unknown command\n";
  }
}

auto debugger::continue_execution() noexcept -> void {
  ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);

  int wait_status;
  auto options = 0;
  waitpid(m_pid, &wait_status, options);
}

auto debugger::set_breakpoint_at_address(std::intptr_t addr) noexcept -> void {
  std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;

  breakpoint bp{m_pid, addr};
  bp.enable();
  m_breakpoints[addr] = bp;
}
