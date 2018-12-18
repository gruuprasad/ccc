#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include "token_type.hpp"
#include <ostream>
#include <string>

namespace ccc {

class Token {
public:
  Token() {}
  Token(ccc::TokenType type, unsigned long line, unsigned long column,
        std::string extra = std::string());
  Token(const Token &t) = default;
  Token &operator=(const Token &t) = default;
  Token(Token &&t) = default;
  Token &operator=(Token &&t) = default;
  ~Token() = default;
  ccc::TokenType getType() const;

  unsigned long getLine() const;
  unsigned long getColumn() const;
  const std::string &getExtra() const;
  const std::string name() const;
  const std::string token_type() const;
  bool is(TokenType expected) const { return type == expected; }
  bool is_not(TokenType expected) const { return type != expected; }
  template <typename T> bool is_oneof(const T &base) const {
    return type == base;
  }
  template <typename T, typename... Args>
  bool is_oneof(const T &first, const Args &... args) const {
    return (type == first) || is_oneof(args...);
  }

  unsigned int getPrecedence();

private:
  ccc::TokenType type;
  unsigned long line;
  unsigned long column;
  std::string extra;

public:
  friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

} // namespace ccc

#endif // C4_TOKEN_HPP
