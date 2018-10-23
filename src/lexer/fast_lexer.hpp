#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include <list>
#include "token.hpp"
#include "lexer_exception.hpp"

class FastLexer {
  std::list<Token, std::allocator<Token>> token_list;
  const std::string content;
  unsigned long position;
  unsigned long line;
  unsigned long column;
  inline bool munch();
  inline char getCharAt(unsigned long position);
  inline bool keyWordEnd(unsigned long position);
  inline bool isKeyword();
public:
  FastLexer(const std::string &content);
  std::list<Token, std::allocator<Token>> lex();
};

#endif //C4_FASTLEXER_H
