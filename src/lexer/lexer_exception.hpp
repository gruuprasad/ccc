#ifndef C4_LEXER_EXCEPTION_HPP
#define C4_LEXER_EXCEPTION_HPP

#include <exception>
#include <string>
#include "token.hpp"

class LexerException : public std::exception {
  const Token token;
  const std::string msg;
  std::basic_string<char, std::char_traits<char>, std::allocator<char>> problem;
public:
  explicit LexerException(const Token &token, std::string msg);
  const char *what() const noexcept override;
};

#endif //C4_LEXER_EXCEPTION_HPP
