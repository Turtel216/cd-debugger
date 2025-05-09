#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <unistd.h>

#include <string>

class debugger {
 public:
  debugger(std::string prog_name, pid_t pid) noexcept
      : m_prog_name{std::move(prog_name)}, m_pid{pid} {}

  auto run() const noexcept -> void;

 private:
  const std::string m_prog_name;
  pid_t m_pid;

  auto handle_command(const std::string& line) const noexcept -> void;
  auto continue_execution() const noexcept -> void;
};

#endif  // DEBUGGER_H_
