/**
 * @file registers.h
 * @brief Defines CPU register types and operations for the debugger.
 *
 * This file contains enumerations, structures, and functions that handle
 * CPU register operations in the debugger, including register value access,
 * mapping between register names and identifiers, and DWARF register number
 * mappings for x86-64 architecture.
 */

#ifndef REGISTERS_H_
#define REGISTERS_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

/**
 * @enum reg
 * @brief Enumeration of CPU registers for x86-64 architecture.
 *
 * This enumeration defines all the CPU registers that can be accessed
 * and manipulated by the debugger, including general-purpose registers,
 * the instruction pointer, segment registers, and flags.
 */
enum class reg {
  rax,      ///< Accumulator register
  rbx,      ///< Base register
  rcx,      ///< Counter register
  rdx,      ///< Data register
  rdi,      ///< Destination index register
  rsi,      ///< Source index register
  rbp,      ///< Base pointer register
  rsp,      ///< Stack pointer register
  r8,       ///< General purpose register R8
  r9,       ///< General purpose register R9
  r10,      ///< General purpose register R10
  r11,      ///< General purpose register R11
  r12,      ///< General purpose register R12
  r13,      ///< General purpose register R13
  r14,      ///< General purpose register R14
  r15,      ///< General purpose register R15
  rip,      ///< Instruction pointer register
  rflags,   ///< CPU flags register
  cs,       ///< Code segment register
  orig_rax, ///< Original RAX value (used by system calls)
  fs_base,  ///< FS segment base address
  gs_base,  ///< GS segment base address
  fs,       ///< FS segment register
  gs,       ///< GS segment register
  ss,       ///< Stack segment register
  ds,       ///< Data segment register
  es        ///< Extra segment register
};

/**
 * @brief Total number of registers defined in the reg enum.
 */
constexpr std::size_t n_registers = 27;

/**
 * @struct reg_descriptor
 * @brief Structure that maps a register to its DWARF number and string
 * representation.
 *
 * This structure associates each register with its corresponding DWARF
 * register number (used in debug information) and human-readable name.
 */
struct reg_descriptor {
  reg r;       ///< The register identifier
  int dwarf_r; ///< Corresponding DWARF register number (-1 if not applicable)
  std::string name; ///< String representation of the register name
};

/**
 * @brief Array of register descriptors for all CPU registers.
 *
 * This global constant array contains descriptors for all CPU registers,
 * providing mappings between register identifiers, DWARF register numbers,
 * and register names. The order matches the order expected by the ptrace
 * system call for register access.
 */
const std::array<reg_descriptor, n_registers> g_register_descriptors{{
    {reg::r15, 15, "r15"},
    {reg::r14, 14, "r14"},
    {reg::r13, 13, "r13"},
    {reg::r12, 12, "r12"},
    {reg::rbp, 6, "rbp"},
    {reg::rbx, 3, "rbx"},
    {reg::r11, 11, "r11"},
    {reg::r10, 10, "r10"},
    {reg::r9, 9, "r9"},
    {reg::r8, 8, "r8"},
    {reg::rax, 0, "rax"},
    {reg::rcx, 2, "rcx"},
    {reg::rdx, 1, "rdx"},
    {reg::rsi, 4, "rsi"},
    {reg::rdi, 5, "rdi"},
    {reg::orig_rax, -1, "orig_rax"},
    {reg::rip, -1, "rip"},
    {reg::cs, 51, "cs"},
    {reg::rflags, 49, "eflags"},
    {reg::rsp, 7, "rsp"},
    {reg::ss, 52, "ss"},
    {reg::fs_base, 58, "fs_base"},
    {reg::gs_base, 59, "gs_base"},
    {reg::ds, 53, "ds"},
    {reg::es, 50, "es"},
    {reg::fs, 54, "fs"},
    {reg::gs, 55, "gs"},
}};

/**
 * @brief Gets the value of a specific register for a process.
 *
 * @param pid Process ID of the target process
 * @param r Register to read
 * @return The current value of the specified register
 */
auto get_register_value(pid_t pid, reg r) noexcept -> std::uint64_t;

/**
 * @brief Gets a register value based on its DWARF register number.
 *
 * This function is useful when working with DWARF debug information
 * that references registers by their DWARF register numbers.
 *
 * @param pid Process ID of the target process
 * @param regnum DWARF register number
 * @return The current value of the register corresponding to the given DWARF
 * number
 */
auto get_register_value_from_dwarf_register(pid_t pid, unsigned regnum)
    -> std::uint64_t;

/**
 * @brief Gets the string name of a register.
 *
 * @param r Register to get the name for
 * @return String representation of the register name
 */
auto get_register_name(reg r) noexcept -> std::string;

/**
 * @brief Gets a register enum value from its string name.
 *
 * @param name String name of the register
 * @return Register enum value corresponding to the given name
 */
auto get_register_from_name(const std::string &name) noexcept -> reg;

/**
 * @brief Sets the value of a specific register for a process.
 *
 * @param pid Process ID of the target process
 * @param r Register to modify
 * @param value New value to set in the register
 */
auto set_register_value(pid_t pid, reg r, uint64_t value) -> void;

#endif // REGISTERS_H_
