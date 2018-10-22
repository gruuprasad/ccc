#ifndef C4_LEXER_EXCEPTION_HPP
#define C4_LEXER_EXCEPTION_HPP

#include <exception>
#include "token.hpp"

class LexerException : public std::exception {
  const Token token;
  std::basic_string<char, std::char_traits<char>, std::allocator<char>> problem;
public:
  explicit LexerException(const Token &token);
  const char *what() const noexcept override;
};

#endif //C4_LEXER_EXCEPTION_HPP
