#include <iostream>
#include "lexer_exception.hpp"

LexerException::LexerException(const Token &token, std::string msg) : token(token), msg(msg) {
  problem = std::to_string(token.getLine()) + ":" + std::to_string(token.getColumn()) + ": error: '"
      + msg + "'. Lexing Stopped!";
}

const char *LexerException::what() const noexcept {
  return problem.c_str();
}
