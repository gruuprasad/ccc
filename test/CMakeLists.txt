add_executable(test_lexer
        test_main.cpp
        lexer/lexer_test.cpp
        lexer/lexer_punctuation_test.cpp
        lexer/lexer_keyword_test.cpp
        lexer/lexer_big_test.cpp
        lexer/first_test.cpp
        )

target_link_libraries(test_lexer entry lexer parser)
add_dependencies(check test_lexer)
add_test(NAME lexer COMMAND test_lexer)

add_executable(test_parser
        test_main.cpp
#        parser/parser_test.cpp
#        ast/parser_simple.cpp
#        parser/parser_big_test.cpp
        )

target_link_libraries(test_parser entry lexer parser)
add_dependencies(check test_parser)
add_test(NAME parser COMMAND test_parser)

add_executable(test_ast
        test_main.cpp
        ast/ast_test.cpp
        )

target_link_libraries(test_ast entry lexer parser)
add_dependencies(check test_ast)
add_test(NAME ast COMMAND test_ast)
