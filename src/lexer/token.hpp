#ifndef C4_TOKEN_HPP
#define C4_TOKEN_HPP

#include <string>
#include <ostream>
#include "token_type.hpp"

struct TokenView {
  const char *first;
  unsigned int length;
  TokenView() : first(nullptr), length(0) {}
  TokenView(const char * first, unsigned int length) : first(first), length(length) {}
  bool empty() const { return first == nullptr || length == 0; }
  friend inline std::ostream &operator<<(std::ostream &os, const TokenView &tv) {
    os.write(tv.first, tv.length);
    return os;
  }
};

class Token {
public:
  TokenType getType() const;
  unsigned long getLine() const;
  unsigned long getColumn() const;
  const std::string name() const;
  const std::string token_type() const;
private:
  const TokenType type;
  const unsigned long line;
  const unsigned long column;
public:
  friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

#endif //C4_TOKEN_HPP
