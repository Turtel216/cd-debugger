#ifndef BREAKPOINT_H_
#define BREAKPOINT_H_

#include <unistd.h>

#include <cstdint>

class breakpoint {
 public:
  breakpoint(pid_t pid, std::intptr_t addr) noexcept
      : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{} {}

  auto enable() noexcept -> void;
  auto disable() noexcept -> void;

 private:
  pid_t m_pid;
  std::intptr_t m_addr;
  bool m_enabled;
  uint8_t m_saved_data;
};

#endif  // BREAKPOINT_H_
