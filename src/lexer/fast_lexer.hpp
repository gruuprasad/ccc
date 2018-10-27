#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include <vector>
#include "token.hpp"
#include "lexer_exception.hpp"

class FastLexer {
  std::vector<Token, std::allocator<Token>> token_list;
  const std::string content;
  unsigned long position;
  unsigned long line;
  unsigned long column;
  inline bool munch();
  inline char getCharAt(unsigned long position);
  inline bool keyWordEnd(unsigned long position);
  inline bool isKeyword();
  inline bool isPunctuator();
public:
  explicit FastLexer(const std::string &content);
  std::vector<Token, std::allocator<Token>> lex();
};

#endif //C4_FASTLEXER_H
