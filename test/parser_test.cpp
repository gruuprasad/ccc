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

TEST_CASE("Fast Parser: Struct declaration test") {
  PARSE_VALID("struct { int; };");
  PARSE_VALID("struct { int a; };");
  PARSE_VALID("struct { void a; short b; int c; "
              "char name; };")
  PARSE_VALID("struct { void a; short b; int c; char name;  int * ptr; "
              "};");
  PARSE_VALID("struct A { void a; short b; int c; char "
              "name;  int * ptr; };")
  PARSE_VALID("struct A { void a; short b; int c; char name;  int * "
              "ptr; } list;")
  PARSE_VALID("struct A { void a; short b; int c; char name;  int * "
              "ptr; } * list;");
  PARSE_VALID("struct A { struct B { int; "
              "}; } "
              "list;")
  PARSE_VALID("struct A { struct B { int x; int y; struct C; "
              "struct { struct E { char z; void * p;}; }; }; } list;")
  PARSE_VALID("struct { int; }; struct A { int x; } a;")
  PARSE_VALID("struct A (a) (int a);");
  PARSE_VALID("struct A (* a) (int a);")
  PARSE_VALID("struct A (** a) (int a);")
  PARSE_VALID("struct A (** a) (int a, void * b);")
  PARSE_VALID("char * callA (int a, void * b);")
  PARSE_VALID("char ** callA (int a, void * "
              "b);")
  PARSE_VALID("char ** ((callA)) (int a, "
              "char * b);")
  PARSE_VALID("char ** (callA (int "
              "x)) (int a, "
              "char * b);") // dubious
                            // pass
  PARSE_VALID("char * ((*callA)) (int a, char * b);")
}

DECLARATION_TESTS("void")
DECLARATION_TESTS("char")
DECLARATION_TESTS("short") DECLARATION_TESTS("int")
    DECLARATION_TESTS("struct A")

#define PARSE_VALID_EXPRESSION(language)                                       \
  {                                                                            \
    auto token_list = FastLexer(language).lex();                               \
    auto fp = FastParser(token_list);                                          \
    fp.parse(PARSE_TYPE::EXPRESSION);                                          \
    REQUIRE(fp.fail() == false);                                               \
  }

    // Test simple expressions
    TEST_CASE("Fast Parser:primary expression test") {
  PARSE_VALID_EXPRESSION("a");
  PARSE_VALID_EXPRESSION("100")
  PARSE_VALID_EXPRESSION("\'c\'")
  PARSE_VALID_EXPRESSION("\"string constant\"")
  PARSE_VALID_EXPRESSION("(\"string constant\")")
  PARSE_VALID_EXPRESSION("(((((25000)))))")
  PARSE_VALID_EXPRESSION("(((((variable)))))")
}

TEST_CASE("Fast Parser:unary expression test") {
  PARSE_VALID_EXPRESSION("&a");
  PARSE_VALID_EXPRESSION("*a")
  PARSE_VALID_EXPRESSION("+a");
  PARSE_VALID_EXPRESSION("-a")
  PARSE_VALID_EXPRESSION("!a");
  PARSE_VALID_EXPRESSION("sizeof a")
  PARSE_VALID_EXPRESSION("sizeof (char)")
}

TEST_CASE("Fast Parser:postfix expression test") {
  PARSE_VALID_EXPRESSION("a[100]");
  PARSE_VALID_EXPRESSION("a()")
  PARSE_VALID_EXPRESSION("a()");
  PARSE_VALID_EXPRESSION("a.b")
  PARSE_VALID_EXPRESSION("a->b")
}

TEST_CASE("Fast Parser:binary expression test") {
  PARSE_VALID_EXPRESSION("a[100] + b[100]");
  PARSE_VALID_EXPRESSION("a[100] - b[100]");
  PARSE_VALID_EXPRESSION("a[100] * b[100]")
  PARSE_VALID_EXPRESSION("a[100] && b[100]");
  PARSE_VALID_EXPRESSION("a[100] || b[100]");
  PARSE_VALID_EXPRESSION("a[100] == b[100]")
  PARSE_VALID_EXPRESSION("a[100] != b[100]")
  PARSE_VALID_EXPRESSION("a[100] = b[100]")
  PARSE_VALID_EXPRESSION("a + b + d - c + (e * f)")
  PARSE_VALID_EXPRESSION("a + (b + d) - c + (e * f)")
  PARSE_VALID_EXPRESSION("a && (b + d) && c || (e() * -f)")
  PARSE_VALID_EXPRESSION("a + b * c")
}

TEST_CASE("debug") {}

#define PARSE_VALID_STATEMENT(language)                                        \
  {                                                                            \
    auto token_list = FastLexer(language).lex();                               \
    auto fp = FastParser(token_list);                                          \
    fp.parse(PARSE_TYPE::STATEMENT);                                           \
    REQUIRE(fp.fail() == false);                                               \
  }

#define PARSE_INVALID_STATEMENT(language)                                      \
  {                                                                            \
    auto token_list = FastLexer(language).lex();                               \
    auto fp = FastParser(token_list);                                          \
    fp.parse(PARSE_TYPE::STATEMENT);                                           \
    REQUIRE(fp.fail() == true);                                                \
  }

TEST_CASE("Fast Parser: simple statement test"){
    PARSE_VALID_STATEMENT("{ 1;}")     // expression-statement
    PARSE_VALID_STATEMENT("{ a: 1;}")  // labeled-statement
    PARSE_VALID_STATEMENT("{ int a;}") // declaration in compound statement
    PARSE_VALID_STATEMENT("{}") PARSE_INVALID_STATEMENT("{ 1;")
        PARSE_INVALID_STATEMENT("{ a: 1;") PARSE_INVALID_STATEMENT("{ int a;")
            PARSE_INVALID_STATEMENT("{ ")}

TEST_CASE("Fast Parser: IF-ELSE statement test") {
  PARSE_VALID_STATEMENT("if (\"true\") ; ");
  PARSE_VALID_STATEMENT("if (1) { if (2) return; else return; } else return;")
  PARSE_VALID_STATEMENT("if (1) { if (2) return; else return; }"
                        " else { if (2) return;}");
  PARSE_VALID_STATEMENT("if (1) if (2) ; "
                        "else ;")
  PARSE_INVALID_STATEMENT("if (\"true\")  ")
  PARSE_INVALID_STATEMENT("if (1)  else ;")
  PARSE_INVALID_STATEMENT("if (1) ; else ")
  PARSE_INVALID_STATEMENT("if (1) { if (2) return; else return;  else return;")
      PARSE_INVALID_STATEMENT(
          "if (1 { if (2) return; else return; } else return;")
          PARSE_INVALID_STATEMENT(
              "if (1 { if (2) return; else return; } if return;")
              PARSE_INVALID_STATEMENT("if (1 { else return; } else return;")
}

TEST_CASE("Fast Parser: Jump statement") {
  PARSE_VALID_STATEMENT("{ goto a; }");
  PARSE_VALID_STATEMENT("{ continue; }")
  PARSE_VALID_STATEMENT("{ break; }");
  PARSE_VALID_STATEMENT("{ return; }")
  PARSE_VALID_STATEMENT("{ return 1000; }")
}

TEST_CASE("Fast Parser: loop statement test") {
  PARSE_VALID_STATEMENT("{ while (10) { 1000; } }")
}
