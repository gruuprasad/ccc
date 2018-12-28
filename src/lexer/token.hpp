#include <utility>

#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include "token_type.hpp"
#include <ostream>
#include <string>

namespace ccc {

class Token {
public:
  Token(const TokenType type, const unsigned long line,
        const unsigned long column, std::string extra = "?")
      : type(type), line(line), column(column), extra(std::move(extra)) {}
  explicit Token(const TokenType type, std::string extra = "?")
      : type(type), line(0), column(0), extra(std::move(extra)) {}
  Token() {}
  TokenType getType() const { return type; }
  unsigned long getLine() const { return line; }
  unsigned long getColumn() const { return column + 1; }
  const std::string &getExtra() const { return extra; }

  Token(const Token &t) = default;
  Token &operator=(const Token &t) = default;
  Token(Token &&t) = default;
  Token &operator=(Token &&t) = default;
  ~Token() = default;
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

  friend std::ostream &operator<<(std::ostream &os, const Token &token);

private:
  ccc::TokenType type;
  unsigned long line;
  unsigned long column;
  std::string extra;
};

} // namespace ccc

#endif // C4_TOKEN_HPP
