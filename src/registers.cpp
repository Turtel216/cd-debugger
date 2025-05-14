#include "../include/registers.h"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

auto get_register_value(pid_t pid, reg r) noexcept -> std::uint64_t {
  user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

  auto it =
      std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                   [r](auto &&rd) { return rd.r == r; });

  return *(reinterpret_cast<std::uint64_t *>(&regs) +
           (it - begin(g_register_descriptors)));
}

auto get_register_value_from_dwarf_register(pid_t pid, unsigned regnum)
    -> std::uint64_t {
  auto it =
      std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                   [regnum](auto &&rd) { return rd.dwarf_r == regnum; });

  if (it == end(g_register_descriptors)) {
    throw std::out_of_range("Unknown dwarf register");
  }

  return get_register_value(pid, it->r);
}

auto get_register_name(reg r) noexcept -> std::string {
  auto it =
      std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                   [r](auto &&rd) { return rd.r == r; });
  return it->name;
}

auto get_register_from_name(const std::string &name) noexcept -> reg {
  auto it =
      std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                   [name](auto &&rd) { return rd.name == name; });
  return it->r;
}

auto set_register_value(pid_t pid, reg r, uint64_t value) -> void {
  user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
  auto it =
      std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                   [r](auto &&rd) { return rd.r == r; });

  *(reinterpret_cast<uint64_t *>(&regs) +
    (it - begin(g_register_descriptors))) = value;
  ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
}
