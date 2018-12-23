#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "lexer/fast_lexer.hpp"
#include "lexer/lexer_exception.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace ccc;

#define KEYWORD_TESTS(keyword, token)                                          \
  TEST_CASE("Fast Lexer keyword " #keyword " positive.") {                     \
    auto firstToken = FastLexer(#keyword).lex().front();                       \
    REQUIRE(firstToken.getType() == token);                                    \
    REQUIRE(firstToken.getLine() == 1);                                        \
    REQUIRE(firstToken.getColumn() == 1);                                      \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " positive prev.") {                \
    auto firstToken = FastLexer("  " #keyword).lex().front();                  \
    REQUIRE(firstToken.getType() == token);                                    \
    REQUIRE(firstToken.getLine() == 1);                                        \
    REQUIRE(firstToken.getColumn() == 3);                                      \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " negative prev.") {                \
    auto firstToken = FastLexer("n" #keyword).lex().front();                   \
    REQUIRE(firstToken.getType() == TokenType::IDENTIFIER);                    \
    REQUIRE(firstToken.getLine() == 1);                                        \
    REQUIRE(firstToken.getColumn() == 1);                                      \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " positive cont.") {                \
    auto firstToken = FastLexer(#keyword "+").lex().front();                   \
    REQUIRE(firstToken.getType() == token);                                    \
    REQUIRE(firstToken.getLine() == 1);                                        \
    REQUIRE(firstToken.getColumn() == 1);                                      \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " negative cont.") {                \
    auto firstToken = FastLexer(#keyword "n").lex().front();                   \
    REQUIRE(firstToken.getType() == TokenType::IDENTIFIER);                    \
    REQUIRE(firstToken.getLine() == 1);                                        \
    REQUIRE(firstToken.getColumn() == 1);                                      \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " length") {                        \
    auto token_list = FastLexer(#keyword " n").lex();                          \
    token_list.pop_back();                                                     \
    auto lastToken = token_list.back();                                        \
    REQUIRE(lastToken.getType() == TokenType::IDENTIFIER);                     \
    REQUIRE(lastToken.getLine() == 1);                                         \
    REQUIRE(lastToken.getColumn() == sizeof(#keyword) + 1);                    \
  }                                                                            \
  TEST_CASE("Fast Lexer keyword " #keyword " stringify.") {                    \
    auto firstToken = FastLexer(#keyword).lex().front();                       \
    std::stringstream buffer;                                                  \
    buffer << firstToken;                                                      \
    std::string result = buffer.str();                                         \
    REQUIRE("1:1: keyword " #keyword);                                         \
  }

KEYWORD_TESTS(auto, TokenType::AUTO)
KEYWORD_TESTS(break, TokenType::BREAK)
KEYWORD_TESTS(case, TokenType::CASE)
KEYWORD_TESTS(char, TokenType::CHAR)
KEYWORD_TESTS(const, TokenType::CONST)
KEYWORD_TESTS(continue, TokenType::CONTINUE)
KEYWORD_TESTS(default, TokenType::DEFAULT)
KEYWORD_TESTS(do, TokenType::DO)
KEYWORD_TESTS(double, TokenType::DOUBLE)
KEYWORD_TESTS(else, TokenType::ELSE)
KEYWORD_TESTS(enum, TokenType::ENUM)
KEYWORD_TESTS(extern, TokenType::EXTERN)
KEYWORD_TESTS(float, TokenType::FLOAT)
KEYWORD_TESTS(for, TokenType::FOR)
KEYWORD_TESTS(goto, TokenType::GOTO)
KEYWORD_TESTS(if, TokenType::IF)
KEYWORD_TESTS(inline, TokenType::INLINE)
KEYWORD_TESTS(int, TokenType::INT)
KEYWORD_TESTS(long, TokenType::LONG)
KEYWORD_TESTS(register, TokenType::REGISTER)
KEYWORD_TESTS(restrict, TokenType::RESTRICT)
KEYWORD_TESTS(return, TokenType::RETURN)
KEYWORD_TESTS(short, TokenType::SHORT)
KEYWORD_TESTS(signed, TokenType::SIGNED)
KEYWORD_TESTS(sizeof, TokenType::SIZEOF)
KEYWORD_TESTS(static, TokenType::STATIC)
KEYWORD_TESTS(struct, TokenType::STRUCT)
KEYWORD_TESTS(switch, TokenType::SWITCH)
KEYWORD_TESTS(typedef, TokenType::TYPEDEF)
KEYWORD_TESTS(union, TokenType::UNION)
KEYWORD_TESTS(unsigned, TokenType::UNSIGNED)
KEYWORD_TESTS(void, TokenType::VOID)
KEYWORD_TESTS(volatile, TokenType::VOLATILE)
KEYWORD_TESTS(while, TokenType::WHILE)
KEYWORD_TESTS(_Alignas, TokenType::ALIGN_AS)
KEYWORD_TESTS(_Alignof, TokenType::ALIGN_OF)
KEYWORD_TESTS(_Atomic, TokenType::ATOMIC)
KEYWORD_TESTS(_Bool, TokenType::BOOL)
KEYWORD_TESTS(_Complex, TokenType::COMPLEX)
KEYWORD_TESTS(_Generic, TokenType::GENERIC)
KEYWORD_TESTS(_Imaginary, TokenType::IMAGINARY)
KEYWORD_TESTS(_Noreturn, TokenType::NO_RETURN)
KEYWORD_TESTS(_Static_assert, TokenType::STATIC_ASSERT)
KEYWORD_TESTS(_Thread_local, TokenType::THREAD_LOCAL)
