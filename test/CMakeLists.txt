llvm_map_components_to_libnames(llvm_libs support core irreader)
add_library(test_LLIB OBJECT test_main.cpp)
target_link_libraries(test_LLIB entry lexer parser ast ${llvm_libs})

add_executable(test_lexer
               lexer/lexer_test.cpp
               lexer/lexer_punctuation_test.cpp
               lexer/lexer_keyword_test.cpp
               lexer/lexer_big_test.cpp
               )
target_link_libraries(test_lexer test_LLIB)
add_dependencies(check test_lexer)
add_test(NAME lexer COMMAND test_lexer)

add_executable(test_parser
               parser/parse_declaration.cpp
               parser/parse_expression.cpp
               parser/parse_test_codes.cpp
               )
target_link_libraries(test_parser test_LLIB)
add_dependencies(check test_parser)
add_test(NAME parser COMMAND test_parser)

add_executable(gen_ast
               ast/ast_test.cpp
               )
target_link_libraries(gen_ast test_LLIB)

add_executable(test_prettyPrinter
               pretty_printer/pretty_printer_test.cpp
               pretty_printer/pretty_printer_ast.cpp
               )
target_link_libraries(test_prettyPrinter test_LLIB)
add_dependencies(check test_prettyPrinter)
add_test(NAME pretty_printer COMMAND test_prettyPrinter)

add_executable(test_semantic
               semantic/semanticAnalysis_test.cpp
               )
target_link_libraries(test_semantic test_LLIB)
add_dependencies(check test_semantic)
add_test(NAME semantic COMMAND test_semantic)

add_executable(test_codegen
               codegen/codegen_test.cpp
               )
target_link_libraries(test_codegen test_LLIB)
add_dependencies(check test_codegen)
add_test(NAME codegen COMMAND test_codegen)

add_executable(gen_clang
               codegen/clang_test.cpp
               )
target_link_libraries(gen_clang test_LLIB)

add_executable(test_blackBox
               black_box/black_box.cpp
               )
target_link_libraries(test_blackBox test_LLIB)

add_executable(test_all
               lexer/lexer_test.cpp
               lexer/lexer_punctuation_test.cpp
               lexer/lexer_keyword_test.cpp
               lexer/lexer_big_test.cpp

               parser/parse_declaration.cpp
               parser/parse_expression.cpp
               parser/parse_test_codes.cpp

               pretty_printer/pretty_printer_test.cpp
               pretty_printer/pretty_printer_ast.cpp

               semantic/semanticAnalysis_test.cpp

               codegen/codegen_test.cpp

               black_box/black_box.cpp
               )
target_link_libraries(test_all test_LLIB)
