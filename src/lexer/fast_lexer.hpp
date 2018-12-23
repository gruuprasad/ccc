#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include "../utils/macros.hpp"
#include "token.hpp"
#include <vector>

namespace ccc {

class FastLexer {
  const std::string &content;
  std::string error;
  unsigned long position = 0;
  unsigned long line = 1;
  unsigned long column = 0;
  inline Token munch();
  inline char getCharAt(unsigned long position);
  inline bool keyWordEnd(unsigned long position);
  inline Token failParsing();
  inline Token munchWhitespace();
  inline Token munchLineComment();
  inline Token munchBlockComment();
  inline Token munchNumber();
  inline Token munchIdentifier();
  inline Token munchCharacter();
  inline Token munchString();
  inline Token munchKeyword();
  inline Token munchPunctuator();

public:
  explicit FastLexer(const std::string &content) : content(content) {}
  std::vector<ccc::Token, std::allocator<ccc::Token>> lex();
  Token lex_valid();
  bool fail() const { return !error.empty(); }
  const std::string &getError() const { return error; }
};

} // namespace ccc

#endif // C4_FASTLEXER_H
