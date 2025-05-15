#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <linux/types.h>
#include <string>
#include <unordered_map>
#include <utility>

#include "breakpoint.h"
#include "dwarf/dwarf++.hh"
#include "elf/elf++.hh"
#include <fcntl.h>
#include <sys/stat.h>

class debugger {
public:
  debugger(std::string prog_name, pid_t pid) noexcept
      : m_prog_name{std::move(prog_name)}, m_pid{pid} {
    auto fd = open(m_prog_name.c_str(), O_RDONLY);

    m_elf = elf::elf{elf::create_mmap_loader(fd)};
    m_dwarf = dwarf::dwarf{dwarf::elf::create_loader(m_elf)};
  };

  auto run() noexcept -> void;
  auto set_breakpoint_at_address(std::intptr_t addr) noexcept -> void;

private:
  const std::string m_prog_name;
  pid_t m_pid;
  std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;
  dwarf::dwarf m_dwarf;
  elf::elf m_elf;
  std::uint64_t m_load_address;

  auto handle_command(const std::string &line) noexcept -> void;
  auto continue_execution() noexcept -> void;
  auto dump_registers() -> void;
  auto read_memory(std::uint64_t address) const noexcept -> std::uint64_t;
  auto write_memory(std::uint16_t address, std::uint64_t value) const noexcept
      -> void;
  auto get_pc() const noexcept -> std::uint64_t;
  auto set_pc(std::uint64_t pc) -> void;
  auto step_over_breakpoint() -> void;
  auto wait_for_signal() const noexcept -> void;
  auto initialise_load_address() noexcept -> void;
  auto get_function_from_pc(std::uint64_t pc) -> dwarf::die;
  auto get_line_entry_from_pc(std::uint64_t pc) -> dwarf::line_table::iterator;
  auto offset_load_address(std::uint64_t addr) -> std::uint64_t;
  auto print_source(const std::string &file_name, unsigned line,
                    unsigned n_lines_context) -> void;
};

#endif // DEBUGGER_H_
