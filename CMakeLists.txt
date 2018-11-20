cmake_minimum_required(VERSION 3.12.2)
project(c4)

set(CMAKE_CXX_STANDARD 11)

if (ENABLE_COVERAGE_TESTING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -g -pg -O0 -fprofile-arcs -ftest-coverage")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Ofast")
endif ()

set(SOURCE_FILES
        src/main.cpp
        src/entry/entry_point_handler.cpp src/entry/entry_point_handler.hpp
        src/lexer/token_type.cpp src/lexer/token_type.hpp
        src/lexer/token.cpp src/lexer/token.hpp
        src/lexer/lexer_exception.cpp src/lexer/lexer_exception.hpp
        src/lexer/fast_lexer.cpp src/lexer/fast_lexer.hpp
        src/parser/fast_parser.cpp src/parser/fast_parser.hpp
        src/utils/assert.hpp)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${test/catch.hpp})

list(SUBLIST SOURCE_FILES 1 -1 TEST_SOURCE_FILES)

set(TEST_FILES_ENTRY
        test/test_main.cpp
        test/first_test.cpp
        )

set(TEST_FILES_LEXER
        test/test_main.cpp
        test/lexer_test.cpp
        test/lexer_keyword_test.cpp
        test/lexer_punctuation_test.cpp
        test/lexer_big_test.cpp
        )

set(TEST_FILES_PARSER
        test/test_main.cpp
        test/parser_test.cpp
        test/parser_big_test.cpp
        )

add_executable(test_entry ${TEST_SOURCE_FILES} ${TEST_FILES_ENTRY})
target_link_libraries(test_entry Catch)
add_test(NAME test_entry COMMAND test_entryr)

add_executable(test_lexer ${TEST_SOURCE_FILES} ${TEST_FILES_LEXER})
target_link_libraries(test_lexer Catch)
add_test(NAME test_lexer COMMAND test_lexer)

add_executable(test_parser ${TEST_SOURCE_FILES} ${TEST_FILES_PARSER})
target_link_libraries(test_parser Catch)
add_test(NAME test_parser COMMAND test_parserr)
