#include <iostream>
#include "lexer_exception.hpp"

LexerException::LexerException(const Token &token) : token(token) {
  problem = "Lexer error at " + std::to_string(token.getLine()) + ":" + std::to_string(token.getColumn()) + " at token: '"
      + token.getExtra() + "'. Lexing Stopped!";
}

const char *LexerException::what() const noexcept {
  return problem.c_str();
}