cmake_minimum_required(VERSION 3.12.2)
project(c4)

enable_testing()
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D DEBUG=truee -Wall -Wextra -pedantic -pthread -g -ftest-coverage")

SET(EXECUTABLE_OUTPUT_PATH      "${PROJECT_BINARY_DIR}/bin")
SET(LIBRARY_OUTPUT_PATH         "${PROJECT_BINARY_DIR}/lib")

add_custom_target(check COMMAND ctest --verbose)

include_directories(src/)
add_subdirectory(src/)
add_subdirectory(test/)

