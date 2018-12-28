add_library(test_main OBJECT test_main.cpp)
add_executable(test_lexer
        lexer/lexer_test.cpp
        lexer/lexer_punctuation_test.cpp
        lexer/lexer_keyword_test.cpp
        lexer/lexer_big_test.cpp
        lexer/first_test.cpp
        $<TARGET_OBJECTS:test_main>
        )

target_link_libraries(test_lexer entry lexer parser)
add_dependencies(check test_lexer)
add_test(NAME lexer COMMAND test_lexer)

add_executable(test_parser
        parser/parser_test.cpp
        $<TARGET_OBJECTS:test_main>
#        ast/parser_simple.cpp
#        parser/parser_big_test.cpp
        )

target_link_libraries(test_parser entry lexer parser)
add_dependencies(check test_parser)
add_test(NAME parser COMMAND test_parser)

add_executable(test_ast
        ast/ast_test.cpp
        $<TARGET_OBJECTS:test_main>
        )

target_link_libraries(test_ast entry lexer parser)
add_dependencies(check test_ast)
add_test(NAME ast COMMAND test_ast)
