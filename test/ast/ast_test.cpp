#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>

//#define PRINT

using namespace ccc;

// TEST_CASE("ast struct test") {
//  auto language = "struct { int; }; struct A { int x; } a;";
//  auto token_list = (new FastLexer(language))->lex();
//  auto *fp = new FastParser(token_list);
//  fp->parse();
//  REQUIRE(!fp->fail());
//}

TEST_CASE("ast statement test") {
  auto language = "void foo () { a == 0; a += 3; int b = 0;}";
  auto token_list = (new FastLexer(language))->lex();
  token_list.emplace_back(TokenType::_EOF, -1, -1);
  auto *fp = new FastParser(token_list);
  ASTNode *root = fp->parse(PARSE_TYPE::TRANSLATIONUNIT);
  if (fp->fail())
    std::cout << fp->getError() << std::endl;
  REQUIRE(!fp->fail());
  std::cout << root->toGraph();
  std::ofstream file;
  file.open("ast.gv");
  file << root->toGraph();
  file.close();
}
