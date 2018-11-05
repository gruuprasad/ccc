#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include <vector>
#include "token.hpp"
#include "lexer_exception.hpp"

class FastLexer {
  std::vector<Token, std::allocator<Token>> token_list;
  const std::string & content;
  unsigned long position;
  unsigned long line;
  unsigned long column;
  std::string error;
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
  std::vector<Token, std::allocator<Token>> lex();
  bool fail() const { return !error.empty(); }
  const std::string & getError() const { return error; }
};

#endif //C4_FASTLEXER_H
