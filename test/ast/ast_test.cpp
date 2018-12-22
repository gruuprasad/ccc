#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"

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
  auto language = "{ int a = 0; int a += 12;}";
  auto token_list = (new FastLexer(language))->lex();
  auto *fp = new FastParser(token_list);
  ASTNode *root = fp->parse(PARSE_TYPE::STATEMENT);
  if (fp->fail())
    std::cout << fp->getError() << std::endl;
  REQUIRE(!fp->fail());
  std::cout << root->toGraph();
}
