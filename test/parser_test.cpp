#include "../src/lexer/fast_lexer.hpp"
#include "../src/parser/fast_parser.hpp"
#include "catch.hpp"

#include <string>

//#define PRINT

using namespace ccc;

template <typename T> void print(const T &t) {
#ifdef PRINT
  for (auto &elem : t)
    std::cout << elem << "  ";
  std::cout << "\n";
#endif
}

#define PARSE_VALID(language)                                                  \
  {                                                                            \
    auto token_list = FastLexer(language).lex();                               \
    auto fp = FastParser(token_list);                                          \
    fp.parse();                                                                \
    REQUIRE(fp.fail() == false);                                               \
  }

#define DECLARATION_TESTS(type)                                                \
  TEST_CASE("Fast Parser:empty declaration test " #type) {                     \
    PARSE_VALID(type " ;")                                                     \
    PARSE_VALID(type " a;")                                                    \
    PARSE_VALID(type " (a);")                                                  \
    PARSE_VALID(type " * p;")                                                  \
    PARSE_VALID(type " ** p;")                                                 \
    PARSE_VALID(type " ** (p);")                                               \
    PARSE_VALID(type " ***** (p);")                                            \
    PARSE_VALID(type " a (" + std::string(type) + " a);")                      \
    PARSE_VALID(type " ;" + std::string(type) + " ;")                          \
    PARSE_VALID(type " a;" + std::string(type) + " a;")                        \
    PARSE_VALID(type " (a);" + std::string(type) + " (a);")                    \
    PARSE_VALID(type " * p;" + std::string(type) + " * p;")                    \
    PARSE_VALID(type " ** p;" + std::string(type) + " ** p;")                  \
    PARSE_VALID(type " ** (p);" + std::string(type) + " ** (p);")              \
    PARSE_VALID(type " ***** (p);" + std::string(type) + " ***** (p);")        \
    PARSE_VALID("void a; char b; short c; int d; struct e;")                   \
    PARSE_VALID(type " a (" + std::string(type) +                              \
                " a, void b, char c, short e);")                               \
  }

TEST_CASE("Fast Parser: Struct declaration test"){
    PARSE_VALID("struct { int; };") PARSE_VALID(
        "struct { int a; };") PARSE_VALID("struct { void a; short b; int c; "
                                          "char name; };")
        PARSE_VALID("struct { void a; short b; int c; char name;  int * ptr; "
                    "};") PARSE_VALID("struct A { void a; short b; int c; char "
                                      "name;  int * ptr; };") PARSE_VALID(
            "struct A { void a; short b; int c; char name;  int * ptr; } list;")
            PARSE_VALID("struct A { void a; short b; int c; char name;  int * "
                        "ptr; } * list;") PARSE_VALID(
                "struct A { struct B { int; }; } list;")
                PARSE_VALID(
                    "struct A { struct B { int x; int y; struct C; "
                    "struct { struct E { char z; void * p;}; }; }; } list;")
                    PARSE_VALID("struct { int; }; struct A { int x; } a;")
                        PARSE_VALID("struct A (a) (int a);") PARSE_VALID(
                            "struct A (* a) (int a);")
                            PARSE_VALID("struct A (** a) (int a);") PARSE_VALID(
                                "struct A (** a) (int a, void * b);")
                                PARSE_VALID("char * callA (int a, void * b);")
                                    PARSE_VALID("char ** callA (int a, void * "
                                                "b);") PARSE_VALID(
                                        "char ** ((callA)) (int a, char * b);")
                                        PARSE_VALID(
                                            "char ** (callA (int x)) (int a, "
                                            "char * b);") // dubious pass
    PARSE_VALID("char * ((*callA)) (int a, char * b);")}

/*
TEST_CASE("debug failing test") {
  {
    auto token_list = FastLexer("struct A (a) (int a);").lex();
    auto fp = FastParser(token_list);
    fp.parse();
    REQUIRE(fp.fail() == false);
  }
}
*/

DECLARATION_TESTS("void") DECLARATION_TESTS("char") DECLARATION_TESTS("short")
    DECLARATION_TESTS("int") DECLARATION_TESTS("struct A")
