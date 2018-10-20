#include <utility>

#include <string>
#include "token.hpp"
#include "token_type.hpp"

Token::Token(const TokenType type, const unsigned long line, const unsigned long column, const std::string extra) :
    type(type), line(line), column(column), extra(extra) {};

const TokenType Token::getType() const {
  return type;
}
const unsigned long Token::getLine() const {
  return line;
}
const unsigned long Token::getColumn() const {
  return column;
}
const std::string &Token::getExtra() const {
  return extra;
}

const std::string Token::name() const {
  switch (type) {
  case TokenType::NUMBER: return "number";
  case TokenType::IDENTIFIER:return "identifier";
  case TokenType::STAR:return "star";
  case TokenType::PLUS:return "plus";
  case TokenType::MINUS:return "minus";
  case TokenType::BRACE_OPEN:return "brace open";
  case TokenType::BRACE_CLOSE:return "brace close";
  case TokenType::LINE_BREAK:return "line break";
  case TokenType::WHITESPACE:return "whitespace";
  }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << token.line << ',' << token.column << " \t" << token.extra << "\t" << token.name();
  return os;
}
