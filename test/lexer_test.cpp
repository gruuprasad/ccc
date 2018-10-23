#include <iostream>
#include <fstream>
#include <sstream>
#include "catch.hpp"
#include "../src/lexer/lexer.hpp"
#include "../src/lexer/fast_lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include <iterator>

#define COMPARE(name) \
TEST_CASE("Compare "#name".c to "#name".txt") { \
  REQUIRE(lexing_of(#name".c", to_match(#name".txt"))); \
}

std::vector<std::string> split_lines(const std::string &str) {
  std::stringstream ss(str);
  std::string tmp;
  std::vector<std::string> split;
  while (std::getline(ss, tmp, '\n')) {
    while (tmp[tmp.length() - 1] == '\r' || tmp[tmp.length() - 1] == '\n') {
      tmp.pop_back();
    }
    split.push_back(tmp);
  }
  return split;
}

bool lexing_of(const std::string &filename, const std::string &expected) {
  std::stringstream buffer;
  EntryPointHandler().tokenize(std::ifstream("../examples/" + filename), filename, buffer);
  const auto content = buffer.str();
  if (content != expected) {
    std::vector<std::string> content_lines = split_lines(content);
    std::vector<std::string> expected_lines = split_lines(expected);
    unsigned int counter = 0;
    for (unsigned long i = 0; i < std::max(content_lines.size(), expected_lines.size()); i++) {
      if (content_lines[i] == expected_lines[i]) {
        continue;
      }
      if (i >= expected_lines.size()) {
        std::cerr << filename << ": expected nothing but got \"" << content_lines[i] << "\"." << std::endl;
      } else if (i >= content_lines.size()) {
        std::cerr << filename << ": expected \"" << expected_lines[i] << "\" but got nothing." << std::endl;
      } else if (content_lines[i] != expected_lines[i]) {
        std::cerr << filename << ": expected \"" << expected_lines[i] << "\" but got \"" << content_lines[i] << "\"."
                  << std::endl;
      }
      counter += 1;
      if (counter > 5) {
        std::cerr << std::endl << "Output truncated after 5 lines...";
        return false;
      }
    }

    return counter == 0;
  }
  return true;
}

std::string to_match(const std::string &filename) {
  std::ifstream t("../examples/" + filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

/*
 * Disable old lexer for now...
 *
COMPARE(test)
COMPARE(hello_world)
COMPARE(error)
COMPARE(everything)
COMPARE(comments)
COMPARE(lorem_ipsum)
COMPARE(extra)
COMPARE(transpose)
COMPARE(lots_of_real_code)
 */

TEST_CASE("Lexer Smoke test.") {
  auto token_list = Lexer().lex("{a+z-3*55aa case }}// }}\na a1 +++++ \"aa\"ee");
}

TEST_CASE("Lexer Simple Operator tests.") {
  REQUIRE(Lexer().lex("+").front().getType() == TokenType::PLUS);
  REQUIRE(Lexer().lex("-").front().getType() == TokenType::MINUS);
  REQUIRE(Lexer().lex("++").front().getType() == TokenType::PLUSPLUS);
  REQUIRE(Lexer().lex("--").front().getType() == TokenType::MINUSMINUS);
}

TEST_CASE("Lexer keyword max munch test.") {
  auto token_list = Lexer().lex("automa");
  Token &token = token_list.front();
  REQUIRE(token.getType() == TokenType::IDENTIFIER);
  REQUIRE(token.getExtra() == "automa");
}

TEST_CASE("Lexer keyword prio test.") {
  REQUIRE(Lexer().lex("auto").front().getType() == TokenType::AUTO);
}

TEST_CASE("Fast Lexer number test.") {
  auto firstToken = FastLexer("123").lex().front();
  REQUIRE(firstToken.getType() == TokenType::NUMBER);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "123");
}

TEST_CASE("Fast Lexer number with extra test.") {
  auto token_list = FastLexer("123afg").lex();
  auto firstToken = token_list.front();
  auto secondToken = token_list.back();
  REQUIRE(firstToken.getType() == TokenType::NUMBER);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "123");
  REQUIRE(secondToken.getType() == TokenType::IDENTIFIER);
  REQUIRE(secondToken.getLine() == 1);
  REQUIRE(secondToken.getColumn() == 4);
  REQUIRE(secondToken.getExtra() == "afg");
}

#define KEYWORD_TESTS(keyword, token) \
TEST_CASE("Fast Lexer keyword "#keyword" positive.") { \
  auto firstToken = FastLexer(#keyword).lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" positive prev.") { \
  auto firstToken = FastLexer("  "#keyword).lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 3); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" negative prev.") { \
  auto firstToken = FastLexer("n"#keyword).lex().front(); \
  REQUIRE(firstToken.getType() == TokenType::IDENTIFIER); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" positive cont.") { \
  auto firstToken = FastLexer(#keyword"+").lex().front(); \
  REQUIRE(firstToken.getType() == token); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \
TEST_CASE("Fast Lexer keyword "#keyword" negative cont.") { \
  auto firstToken = FastLexer(#keyword"n").lex().front(); \
  REQUIRE(firstToken.getType() == TokenType::IDENTIFIER); \
  REQUIRE(firstToken.getLine() == 1); \
  REQUIRE(firstToken.getColumn() == 1); \
} \

KEYWORD_TESTS(auto, TokenType::AUTO)
KEYWORD_TESTS(break, TokenType::BREAK)
KEYWORD_TESTS(case, TokenType::CASE)
KEYWORD_TESTS(char, TokenType::CHAR)
KEYWORD_TESTS(const, TokenType::CONST)
KEYWORD_TESTS(continue, TokenType::CONTINUE)
KEYWORD_TESTS(default, TokenType::DEFAULT)
KEYWORD_TESTS(do, TokenType::DO)
KEYWORD_TESTS(else, TokenType::ELSE)
KEYWORD_TESTS(enum, TokenType::ENUM)
KEYWORD_TESTS(extern, TokenType::EXTERN)
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
KEYWORD_TESTS(union, TokenType::UNION)
KEYWORD_TESTS(unsigned, TokenType::UNSIGNED)
KEYWORD_TESTS(void, TokenType::VOID)
KEYWORD_TESTS(volatile, TokenType::VOLATILE)
KEYWORD_TESTS(while, TokenType::WHILE)


