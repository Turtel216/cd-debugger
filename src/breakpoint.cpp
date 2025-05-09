#include "../include/breakpoint.hpp"

#include <sys/ptrace.h>
#include <unistd.h>

#include <cstdint>

auto breakpoint::enable() noexcept -> void {
  pid_t data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
  m_saved_data = static_cast<uint8_t>(data & 0xff);
  uint64_t int3 = 0xcc;
  uint64_t data_with_int3 = ((data & ~0xff) | int3);

  ptrace(PTRACE_POKEDATA, m_pid, m_addr, data_with_int3);

  m_enabled = true;
}

auto breakpoint::disable() noexcept -> void {
  pid_t data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
  auto restored_data = ((data & ~0xff) | m_saved_data);
  ptrace(PTRACE_POKEDATA, m_pid, restored_data);

  m_enabled = false;
}
