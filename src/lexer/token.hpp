#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include <string>
#include <ostream>
#include "token_type.hpp"

class Token {
public:
  Token(TokenType type, unsigned long line, unsigned long column, std::string extra);
  const TokenType getType() const;
  const unsigned long getLine() const;
  const unsigned long getColumn() const;
  const std::string &getExtra() const;
  const std::string name() const;
private:
  const TokenType type;
  const unsigned long line;
  const unsigned long column;
  const std::string extra;
public:
  friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

#endif //C4_TOKEN_HPP
