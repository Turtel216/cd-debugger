#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <linux/types.h>

#include <string>
#include <unordered_map>
#include <utility>

#include "breakpoint.h"

class debugger {
public:
  debugger(std::string prog_name, pid_t pid) noexcept
      : m_prog_name{std::move(prog_name)}, m_pid{pid} {}

  auto run() noexcept -> void;
  auto set_breakpoint_at_address(std::intptr_t addr) noexcept -> void;

private:
  const std::string m_prog_name;
  pid_t m_pid;
  std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;

  auto handle_command(const std::string &line) noexcept -> void;
  auto continue_execution() noexcept -> void;
};

#endif // DEBUGGER_H_
