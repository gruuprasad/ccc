#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include "lexer_exception.hpp"
#include "token.hpp"
#include <vector>

namespace ccc {

class FastLexer {
  std::vector<ccc::Token, std::allocator<ccc::Token>> token_list;
  const std::string &content;
  std::string error;
  unsigned long position = 0;
  unsigned long line = 1;
  unsigned long column = 0;
  inline bool munch();
  inline char getCharAt(unsigned long position);
  inline bool keyWordEnd(unsigned long position);
  inline bool failParsing();
  inline bool munchWhitespace();
  inline bool munchLineComment();
  inline bool munchBlockComment();
  inline bool munchNumber();
  inline bool munchIdentifier();
  inline bool munchCharacter();
  inline bool munchString();
  inline bool isKeyword();
  inline bool isPunctuator();

public:
  explicit FastLexer(const std::string &content);
  std::vector<ccc::Token, std::allocator<ccc::Token>> lex();
  bool fail() const { return !error.empty(); }
  const std::string &getError() const { return error; }
};

} // namespace ccc

#endif // C4_FASTLEXER_H
