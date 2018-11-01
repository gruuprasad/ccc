cmake_minimum_required(VERSION 3.12.2)
project(c4)

set(CMAKE_CXX_STANDARD 11)

if (ENABLE_COVERAGE_TESTING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -g -pg -O0 -fprofile-arcs -ftest-coverage")
endif ()

set(SOURCE_FILES
        src/main.cpp
        src/entry/entry_point_handler.cpp src/entry/entry_point_handler.hpp
        src/lexer/token_type.cpp src/lexer/token_type.hpp
        src/lexer/token.cpp src/lexer/token.hpp
        src/lexer/lexer_exception.cpp src/lexer/lexer_exception.hpp
        src/lexer/fast_lexer.cpp src/lexer/fast_lexer.hpp
        src/ast/statement.cpp src/ast/statement.hpp
        src/ast/expression.cpp src/ast/expression.hpp
        src/ast/ast_node.cpp src/ast/ast_node.hpp
        src/ast/declaration.cpp src/ast/declaration.hpp
        src/ast/constant.cpp src/ast/constant.hpp
        src/ast/type_specifier.cpp src/ast/type_specifier.hpp
        )

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${test/catch.hpp})

list(SUBLIST SOURCE_FILES 1 -1 TEST_SOURCE_FILES)

set(TEST_FILES
        test/test_main.cpp
        test/first_test.cpp
        test/lexer_test.cpp
        test/lexer_keyword_test.cpp
        test/lexer_punctuation_test.cpp
        test/lexer_big_test.cpp
        )

add_executable(tests ${TEST_SOURCE_FILES} ${TEST_FILES})
target_link_libraries(tests Catch)
add_test(NAME tests COMMAND tests)
