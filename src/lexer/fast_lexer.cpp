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

inline bool FastLexer::keyWordEnd(unsigned long position) {
  const char first = getCharAt(position);
  return !(('0' <= first && first <= '9')
      || ('a' <= first && first <= 'z')
      || ('A' <= first && first <= 'Z')
      || first == '_');
}

inline bool FastLexer::isKeyword() {
  const char first = getCharAt(position);
  switch (first) {
  case 'a':
    if (getCharAt(position + 1) == 'u'
        && getCharAt(position + 2) == 't'
        && getCharAt(position + 3) == 'o'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(Token(TokenType::AUTO, line, column, ""));
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'b':
    if (getCharAt(position + 1) == 'r'
        && getCharAt(position + 2) == 'e'
        && getCharAt(position + 3) == 'a'
        && getCharAt(position + 4) == 'k'
        && keyWordEnd(position + 5)
        ) {
      token_list.emplace_back(Token(TokenType::BREAK, line, column, ""));
      position += 5;
      column += 5;
      return true;
    }
    break;
  default:
    break;
  }
  /*
   * Fallthrough, no keyword matched!
   */
  return false;
}

bool FastLexer::munch() {
  char first = getCharAt(position);
  /*
   * At end of file, return
   */
  if (first == 0) {
    return false;
  }
  /*
   * Munch away all whitespace
   */
  while (first == ' '
      || first == '\t'
      || first == '\n'
      || first == '\r'
      ) {
    first = getCharAt(++position);
  }
  unsigned long oldPosition = position;
  std::stringstream tokenStream;
  /*
   * Check if we have a number at hand
   */
  if ('0' <= first && first <= '9') {
    do {
      tokenStream << first;
      first = getCharAt(++position);
    } while ('0' <= first && first <= '9');
    token_list.emplace_back(Token(TokenType::NUMBER, line, column, tokenStream.str()));
    column += position - oldPosition;
    return true;
  }
  /*
   * Check if we have an identifier or a keyword at hand
   */
  if (('a' <= first && first <= 'z')
      || ('A' <= first && first <= 'Z')
      || first == '_') {
    if (isKeyword()) {
      /*
       * We found a keyword already munched, return
       */
      return true;
    }
    /*
     * No keyword, check for identifier
     */
    do {
      tokenStream << first;
      first = getCharAt(++position);
    } while (('0' <= first && first <= '9')
        || ('a' <= first && first <= 'z')
        || ('A' <= first && first <= 'Z')
        || first == '_');
    token_list.emplace_back(Token(TokenType::IDENTIFIER, line, column, tokenStream.str()));
    column += position - oldPosition;
    return true;
  }
  /*
   * We matched nothing, we should fail the lexing!
   */
  return false;
}

std::list<Token, std::allocator<Token>> FastLexer::lex() {
  while (munch()) {
  }
  return token_list;
}


