#include "lexer_exception.hpp"
#include <iostream>

namespace ccc {

LexerException::LexerException(const Token &token) : token(token) {
  problem = std::to_string(token.getLine()) + ":" +
            std::to_string(token.getColumn()) + ": error: '" +
            token.getExtra() + "'. Lexing Stopped!";
}

const char *LexerException::what() const noexcept { return problem.c_str(); }

} // namespace ccc
