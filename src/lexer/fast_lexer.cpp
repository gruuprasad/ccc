#include <sstream>
#include "fast_lexer.hpp"
FastLexer::FastLexer(const std::string &content) : content(content) {
  token_list = std::list<Token, std::allocator<Token>>();
  position = 0;
  line = 1;
  column = 0;
}

inline char FastLexer::getCharAt(unsigned long position) {
  return content[position];
}

bool FastLexer::munch() {
  char first = getCharAt(position);
  if(first == 0){
    return false;
  }
  unsigned long oldPosition = position;
  std::stringstream tokenStream;
  if ('0' <= first && first <= '9') {
    do {
      tokenStream << first;
      first = getCharAt(++position);
    } while ('0' <= first && first <= '9');
    token_list.emplace_back(Token(TokenType::NUMBER, line, column, tokenStream.str()));
    column += position - oldPosition;
    return true;
  }
  return false;
}

std::list<Token, std::allocator<Token>> FastLexer::lex() {
  while(munch()){}
  return token_list;
}


