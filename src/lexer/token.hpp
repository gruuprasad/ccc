#include <utility>

#include <utility>

#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include "../utils/location.hpp"
#include "token_type.hpp"
#include <ostream>
#include <string>

namespace ccc {

using Precedence = unsigned int;

class Token {
public:
  Token() : type(TokenType::GHOST), loc(0, 0), extra(std::string()) {}
  explicit Token(TokenType type, std::string extra = std::string())
      : type(type), loc(0, 0), extra(std::move(extra)) {}
  Token(const TokenType type, const unsigned long line,
        const unsigned long column,
        std::string extra = std::string()) // deprecated, replace w/ Location
      : type(type), loc(line, column), extra(std::move(extra)) {}
  Token(const TokenType type, const Location loc,
        std::string extra = std::string())
      : type(type), loc(loc), extra(std::move(extra)) {}
  Token(const Token &t) = default;
  Token &operator=(const Token &t) = default;
  Token(Token &&t) = default;
  Token &operator=(Token &&t) = default;
  ~Token() = default;

  TokenType getType() const { return type; }
  unsigned long getLine() const { return loc.getLine(); }
  unsigned long getColumn() const { return loc.getColumn(); }
  const Location &getLocation() const { return loc; }
  const std::string &getExtra() const { return extra; }
  const std::string name() const;
  const std::string token_type() const;
  bool isGhostType() const { return type == TokenType::GHOST; }

  bool is_not(TokenType expected) const { return type != expected; }
  template <typename T> bool is(const T &base) const { return type == base; }
  template <typename T, typename... Args>
  bool is(const T &first, const Args &... args) const {
    return (type == first) || is(args...);
  }
  Precedence getPrecedence() const;

  friend std::ostream &operator<<(std::ostream &os, const Token &token);

private:
  TokenType type;
  Location loc;
  std::string extra;
};

} // namespace ccc

#endif // C4_TOKEN_HPP
