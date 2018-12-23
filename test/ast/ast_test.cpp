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
  auto language = "{ a == 0; a += 3; int b = 0;}";

  auto token_list = (new FastLexer(language))->lex();
  token_list.emplace_back(TokenType::ENDOFFILE, -1, -1);

  auto *fp = new FastParser(token_list);

  auto *root = fp->parse(PARSE_TYPE::TRANSLATIONUNIT, true);
  REQUIRE(!fp->fail());

  //  std::cout << root->toGraph();
  std::ofstream file;
  file.open("ast.gv");
  file << root->toGraph();
  file.close();
}
