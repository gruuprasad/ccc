#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include <string>
#include <ostream>
#include "token_type.hpp"

class Token {
public:
  Token(TokenType type, unsigned long line, unsigned long column, std::string extra = std::string());
  TokenType getType() const;
  unsigned long getLine() const;
  unsigned long getColumn() const;
  const std::string &getExtra() const;
  const std::string name() const;
  const std::string token_type() const;
private:
  const TokenType type;
  const unsigned long line;
  const unsigned long column;
  std::string extra;
public:
  friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

#endif //C4_TOKEN_HPP
