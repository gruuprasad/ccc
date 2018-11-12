#include "catch.hpp"
#include "../src/parser/fast_parser.hpp"
#include "../src/lexer/fast_lexer.hpp"

//#define PRINT

using namespace ccc;

template<typename T>
void print(const T& t) {
#ifdef PRINT
  for (auto & elem : t)
    std::cout << elem << "  ";
  std::cout << "\n";
#endif
}

#define PARSE_VALID(language) \
  {\
    auto token_list = FastLexer(language).lex(); \
    auto fp = FastParser(token_list); \
    fp.parse(); \
    REQUIRE(fp.fail() == false); \
  }\

#define DECLARATION_TESTS(type) \
TEST_CASE("Fast Parser:empty declaration test "#type) { \
  PARSE_VALID(type" ;") \
} \
\
TEST_CASE("Fast Parser: declaration test "#type) { \
  PARSE_VALID(type" a;")\
  PARSE_VALID(type" (a);")\
}\
\
TEST_CASE("Fast Parser: pointer declarator test "#type) { \
  PARSE_VALID(type" * p;")\
  PARSE_VALID(type" ** p;")\
  PARSE_VALID(type" ** (p);")\
  PARSE_VALID(type" ***** (p);")\
}\

TEST_CASE("Fast Parser: Struct declaration test") {
  PARSE_VALID("struct { int; };")
  PARSE_VALID("struct { int a; };")
  PARSE_VALID("struct { void a; short b; int c; char name; };")
  PARSE_VALID("struct { void a; short b; int c; char name;  int * ptr; };")
  PARSE_VALID("struct A { void a; short b; int c; char name;  int * ptr; };")
  PARSE_VALID("struct A { void a; short b; int c; char name;  int * ptr; } list;")
  PARSE_VALID("struct A { void a; short b; int c; char name;  int * ptr; } * list;")
}

DECLARATION_TESTS("void")
DECLARATION_TESTS("char")
DECLARATION_TESTS("short")
DECLARATION_TESTS("int")
DECLARATION_TESTS("struct A")
