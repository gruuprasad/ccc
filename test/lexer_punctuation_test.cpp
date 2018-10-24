#include <iostream>
#include <fstream>
#include <sstream>
#include "catch.hpp"
#include "../src/lexer/lexer.hpp"
#include "../src/lexer/fast_lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/lexer/lexer_exception.hpp"
#include <iterator>

#define PUNCTUATOR_TESTS(keyword, token) \
TEST_CASE("Fast Lexer keyword "#keyword" positive.") { \
  auto firstToken = FastLexer(keyword).lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" positive prev.") { \
  auto firstToken = FastLexer("  " keyword).lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 3); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" positive cont.") { \
  auto firstToken = FastLexer(keyword"d").lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \

PUNCTUATOR_TESTS("{", TokenType::BRACE_OPEN)
PUNCTUATOR_TESTS("}", TokenType::BRACE_CLOSE)
PUNCTUATOR_TESTS("[", TokenType::BRACKET_OPEN)
PUNCTUATOR_TESTS("]", TokenType::BRACKET_CLOSE)
PUNCTUATOR_TESTS("(", TokenType::PARENTHESIS_OPEN)
PUNCTUATOR_TESTS(")", TokenType::PARENTHESIS_CLOSE)
PUNCTUATOR_TESTS("+=", TokenType::PLUS_ASSIGN)
PUNCTUATOR_TESTS("++", TokenType::PLUSPLUS)
PUNCTUATOR_TESTS("+", TokenType::PLUS)
PUNCTUATOR_TESTS("-=", TokenType::MINUS_ASSIGN)
PUNCTUATOR_TESTS("--", TokenType::MINUSMINUS)
PUNCTUATOR_TESTS("->*", TokenType::ARROW_STAR)
PUNCTUATOR_TESTS("->", TokenType::ARROW)
PUNCTUATOR_TESTS("-", TokenType::MINUS)
PUNCTUATOR_TESTS("==", TokenType::EQUAL)
PUNCTUATOR_TESTS("=", TokenType::ASSIGN)
PUNCTUATOR_TESTS("<=", TokenType::LESS_EQUAL)
PUNCTUATOR_TESTS("<<=", TokenType::LEFT_SHIFT_ASSIGN)
PUNCTUATOR_TESTS("<<", TokenType::LEFT_SHIFT)
PUNCTUATOR_TESTS("<", TokenType::LEFT)
PUNCTUATOR_TESTS(">=", TokenType::GREATER_EQUAL)
PUNCTUATOR_TESTS(">>=", TokenType::RIGHT_SHIFT_ASSIGN)
PUNCTUATOR_TESTS(">>", TokenType::RIGHT_SHIFT)
PUNCTUATOR_TESTS(">", TokenType::RIGHT)
PUNCTUATOR_TESTS("!=", TokenType::NOT_EQUAL)
PUNCTUATOR_TESTS("!", TokenType::NOT)
PUNCTUATOR_TESTS(",", TokenType::COMMA)
PUNCTUATOR_TESTS(";", TokenType::SEMICOLON)
PUNCTUATOR_TESTS("...", TokenType::TRI_DOTS)
PUNCTUATOR_TESTS(".*", TokenType::DOT_STAR)
PUNCTUATOR_TESTS(".", TokenType::DOT)
PUNCTUATOR_TESTS("^=", TokenType::CARET_ASSIGN)
PUNCTUATOR_TESTS("^", TokenType::CARET)
PUNCTUATOR_TESTS("~", TokenType::TILDE)
PUNCTUATOR_TESTS("*=", TokenType::STAR_ASSIGN)
PUNCTUATOR_TESTS("*", TokenType::STAR)
PUNCTUATOR_TESTS("/=", TokenType::DIV_ASSIGN)
PUNCTUATOR_TESTS("/", TokenType::DIV)
PUNCTUATOR_TESTS("%=", TokenType::MOD_ASSIGN)
PUNCTUATOR_TESTS("%", TokenType::MOD)
PUNCTUATOR_TESTS("&=", TokenType::AMPERSAND_ASSIGN)
PUNCTUATOR_TESTS("&&", TokenType::AND)
PUNCTUATOR_TESTS("&", TokenType::AMPERSAND)
PUNCTUATOR_TESTS("|=", TokenType::PIPE_ASSIGN)
PUNCTUATOR_TESTS("||", TokenType::OR)
PUNCTUATOR_TESTS("|", TokenType::PIPE)
PUNCTUATOR_TESTS("::", TokenType::COLON_COLON)
PUNCTUATOR_TESTS(":", TokenType::COLON)
PUNCTUATOR_TESTS("?", TokenType::QUESTION)
