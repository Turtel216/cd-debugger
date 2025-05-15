cmake_minimum_required (VERSION 3.10)
project (cdb)
add_compile_options(-std=c++14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(external/libelfin external/linenoise include)
add_executable(cdb src/main.cpp src/debugger.cpp src/breakpoint.cpp src/registers.cpp external/linenoise/linenoise.c)

add_executable(hello examples/hello.cpp)
set_target_properties(hello
                      PROPERTIES COMPILE_FLAGS "-g -O0")

add_executable(variable examples/variable.cpp)
set_target_properties(variable
                      PROPERTIES COMPILE_FLAGS "-gdwarf-2 -O0")

add_executable(unwinding examples/stack_unwinding.cpp)
set_target_properties(unwinding
                      PROPERTIES COMPILE_FLAGS "-g -O0")


add_custom_target(
   libelfin
   COMMAND make
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external/libelfin
)
target_link_libraries(cdb
                      ${PROJECT_SOURCE_DIR}/external/libelfin/dwarf/libdwarf++.so
                      ${PROJECT_SOURCE_DIR}/external/libelfin/elf/libelf++.so)
add_dependencies(cdb libelfin)
