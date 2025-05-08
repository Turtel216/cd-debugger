cmake_minimum_required(VERSION 3.30)
project(cdb LANGUAGES CXX)

# Enable testing
include(CTest)

# Source files
set(SRC_FILES
    src/main.cpp
)


# Library for parser
# add_library(parser_lib
#     src/utils/errors/errors.cpp
#     src/lexer/lexer.cpp
#     src/lexer/token.cpp
#     src/parser/ast.cpp
#     src/parser/parser.cpp)

# Main executable
add_executable(cdb ${SRC_FILES})

# Tests
if (BUILD_TESTING)
    # add_executable(lexer_test_success tests/lexer/test_success.cpp)
    # target_link_libraries(lexer_test_success lexer_lib)
    # add_test(NAME parser_failure COMMAND parser_test_failure)
endif()

# C++ standard
set_target_properties(cdb PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
