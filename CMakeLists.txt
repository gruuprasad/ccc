cmake_minimum_required(VERSION 3.12.2)
project(c4)

enable_testing()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(LLVM 7 REQUIRED CONFIG PATHS "${PROJECT_SOURCE_DIR}/llvm/install/lib/cmake/")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -pedantic -pthread -g3")


SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

add_custom_target(check COMMAND ctest --verbose)

include_directories(src/)
add_subdirectory(src/)
add_subdirectory(test/)
add_subdirectory(LLVM_Examples/)

