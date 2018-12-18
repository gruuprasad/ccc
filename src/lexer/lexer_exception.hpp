#ifndef C4_LEXER_EXCEPTION_HPP
#define C4_LEXER_EXCEPTION_HPP

#include "token.hpp"
#include <exception>

namespace ccc {

class LexerException : public std::exception {
  const ccc::Token token;
  std::basic_string<char, std::char_traits<char>, std::allocator<char>> problem;

public:
  explicit LexerException(const ccc::Token &token);
  const char *what() const noexcept override;
};

} // namespace ccc
#endif // C4_LEXER_EXCEPTION_HPP
