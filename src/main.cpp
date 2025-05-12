#include <sys/personality.h>
#include <sys/ptrace.h>
#include <unistd.h>

#include <iostream>

#include "../include/debugger.h"

auto execute_debugee(const std::string& prog_name) noexcept -> void;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Program name not specified\n";
    return -1;
  }

  char* prog = argv[1];

  pid_t pid = fork();
  if (pid == 0) {  // child process
    personality(ADDR_NO_RANDOMIZE);
    execute_debugee(prog);
  } else if (pid >= 1) {  // parent process
    std::cout << "Started debugging process " << pid << '\n';
    debugger dbg{prog, pid};
    dbg.run();
  }
}

auto execute_debugee(const std::string& prog_name) noexcept -> void {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    std::cerr << "Error in ptrace\n";
    return;
  }
  execl(prog_name.c_str(), prog_name.c_str(), nullptr);
}
