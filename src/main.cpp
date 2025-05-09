#include <unistd.h>

#include <iostream>

#include "../include/debugger.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Program name not specified\n";
    return -1;
  }

  char* prog = argv[1];

  pid_t pid = fork();
  if (pid == 0) {         // child process
  } else if (pid >= 1) {  // parent process
    std::cout << "Started debugging process " << pid << '\n';
    debugger dbg{prog, pid};
    dbg.run();
  }
}
