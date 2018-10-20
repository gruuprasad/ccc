#ifndef C4_TOKEN_TYPE_HPP
#define C4_TOKEN_TYPE_HPP

#include <string>
enum class TokenType {
  NUMBER,
  IDENTIFIER,
  STAR,
  PLUS,
  MINUS,
  BRACE_OPEN,
  BRACE_CLOSE,
  LINE_BREAK,
  WHITESPACE,
};

#endif //C4_TOKEN_TYPE_HPP
