/**
 * @file breakpoint.h
 * @brief Defines a class for handling debugger breakpoints.
 *
 * This file contains the breakpoint class which manages setting, enabling,
 * and disabling breakpoints for a debugger. It handles the low-level details
 * of saving and restoring instruction data when breakpoints are toggled.
 */

#ifndef BREAKPOINT_H_
#define BREAKPOINT_H_

#include <cstdint>
#include <unistd.h>

/**
 * @class breakpoint
 * @brief Represents a single breakpoint in a debugging session.
 *
 * The breakpoint class manages the setting and clearing of breakpoints
 * for a specific process ID at a specific memory address. It saves the
 * original data at the breakpoint location to allow for restoration when
 * the breakpoint is disabled.
 */
class breakpoint {
public:
  /**
   * @brief Default constructor.
   */
  breakpoint() = default;

  /**
   * @brief Constructs a breakpoint for a specific process at a specific
   * address.
   *
   * @param pid The process ID to set the breakpoint for.
   * @param addr The memory address where the breakpoint should be placed.
   */
  breakpoint(pid_t pid, std::intptr_t addr) noexcept
      : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{} {}

  /**
   * @brief Enables the breakpoint.
   *
   * When enabled, this method saves the original data at the breakpoint address
   * and replaces it with an interrupt instruction (typically 0xCC on x86).
   */
  auto enable() noexcept -> void;

  /**
   * @brief Disables the breakpoint.
   *
   * When disabled, this method restores the original data that was saved
   * when the breakpoint was enabled.
   */
  auto disable() noexcept -> void;

  /**
   * @brief Checks if the breakpoint is currently enabled.
   *
   * @return true if the breakpoint is enabled, false otherwise.
   */
  auto is_enabled() const noexcept -> bool;

private:
  pid_t m_pid;          ///< The process ID this breakpoint applies to
  std::intptr_t m_addr; ///< The memory address of this breakpoint
  bool m_enabled;       ///< Current state of the breakpoint (enabled/disabled)
  uint8_t
      m_saved_data; ///< The original byte that was at the breakpoint address
};

#endif // BREAKPOINT_H_
