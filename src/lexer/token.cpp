#include <utility>

#include "token.hpp"
#include <iostream>
#include <string>

namespace ccc {

Token::Token(const TokenType type, const unsigned long line,
             const unsigned long column, const std::string extra)
    : type(type), line(line), column(column), extra(std::move(extra)) {}

TokenType Token::getType() const { return type; }
unsigned long Token::getLine() const { return line; }
unsigned long Token::getColumn() const { return column + 1; }
const std::string &Token::getExtra() const { return extra; }

const std::string Token::name() const {
  switch (type) {
  case TokenType::NUMBER:
    return "number";
  case TokenType::IDENTIFIER:
    return "identifier";
  case TokenType::STAR:
    return "*";
  case TokenType::PLUS:
    return "+";
  case TokenType::PLUSPLUS:
    return "++";
  case TokenType::MINUS:
    return "-";
  case TokenType::BRACE_OPEN:
    return "{";
  case TokenType::BRACE_CLOSE:
    return "}";
  case TokenType::AUTO:
    return "auto";
  case TokenType::BREAK:
    return "break";
  case TokenType::CASE:
    return "case";
  case TokenType::CHAR:
    return "char";
  case TokenType::CONST:
    return "const";
  case TokenType::CONTINUE:
    return "continue";
  case TokenType::DEFAULT:
    return "default";
  case TokenType::DO:
    return "do";
  case TokenType::ELSE:
    return "else";
  case TokenType::ENUM:
    return "enum";
  case TokenType::EXTERN:
    return "extern";
  case TokenType::FOR:
    return "for";
  case TokenType::GOTO:
    return "goto";
  case TokenType::IF:
    return "if";
  case TokenType::INLINE:
    return "inline";
  case TokenType::INT:
    return "int";
  case TokenType::LONG:
    return "long";
  case TokenType::REGISTER:
    return "register";
  case TokenType::RESTRICT:
    return "restrict";
  case TokenType::RETURN:
    return "return";
  case TokenType::SHORT:
    return "short";
  case TokenType::SIGNED:
    return "signed";
  case TokenType::SIZEOF:
    return "sizeof";
  case TokenType::STATIC:
    return "static";
  case TokenType::STRUCT:
    return "struct";
  case TokenType::SWITCH:
    return "switch";
  case TokenType::TYPEDEF:
    return "typedef";
  case TokenType::UNION:
    return "union";
  case TokenType::UNSIGNED:
    return "unsigned";
  case TokenType::VOID:
    return "void";
  case TokenType::VOLATILE:
    return "volatile";
  case TokenType::WHILE:
    return "while";
  case TokenType::ALIGN_AS:
    return "_Alignas";
  case TokenType::ALIGN_OF:
    return "_Alignof";
  case TokenType::ATOMIC:
    return "_Atomic";
  case TokenType::BOOL:
    return "_Bool";
  case TokenType::COMPLEX:
    return "_Complex";
  case TokenType::GENERIC:
    return "_Generic";
  case TokenType::IMAGINARY:
    return "_Imaginary";
  case TokenType::NO_RETURN:
    return "_Noreturn";
  case TokenType::STATIC_ASSERT:
    return "_Static_assert";
  case TokenType::THREAD_LOCAL:
    return "_Thread_local";
  case TokenType::BRACKET_OPEN:
    return "[";
  case TokenType::BRACKET_CLOSE:
    return "]";
  case TokenType::PARENTHESIS_OPEN:
    return "(";
  case TokenType::PARENTHESIS_CLOSE:
    return ")";
  case TokenType::AMPERSAND:
    return "&";
  case TokenType::PIPE:
    return "|";
  case TokenType::CARET:
    return "^";
  case TokenType::TILDE:
    return "~";
  case TokenType::LEFT_SHIFT:
    return "<<";
  case TokenType::RIGHT_SHIFT:
    return ">>";
  case TokenType::GREATER_EQUAL:
    return ">=";
  case TokenType::LESS_EQUAL:
    return "<=";
  case TokenType::EQUAL:
    return "==";
  case TokenType::ASSIGN:
    return "=";
  case TokenType::MINUSMINUS:
    return "--";
  case TokenType::DIV:
    return "/";
  case TokenType::MOD:
    return "%";
  case TokenType::PLUS_ASSIGN:
    return "+=";
  case TokenType::MINUS_ASSIGN:
    return "-=";
  case TokenType::AMPERSAND_ASSIGN:
    return "&=";
  case TokenType::PIPE_ASSIGN:
    return "|=";
  case TokenType::CARET_ASSIGN:
    return "^=";
  case TokenType::STAR_ASSIGN:
    return "*=";
  case TokenType::DIV_ASSIGN:
    return "/=";
  case TokenType::MOD_ASSIGN:
    return "%=";
  case TokenType::STRING:
    return "STRING";
  case TokenType::LESS:
    return "<";
  case TokenType::GREATER:
    return ">";
  case TokenType::NOT_EQUAL:
    return "!=";
  case TokenType::NOT:
    return "!";
  case TokenType::ARROW:
    return "->";
  case TokenType::COMMA:
    return ",";
  case TokenType::COLON:
    return ":";
  case TokenType::QUESTION:
    return "?";
  case TokenType::SEMICOLON:
    return ";";
  case TokenType::TRI_DOTS:
    return "...";
  case TokenType::DOT:
    return ".";
  case TokenType::AND:
    return "&&";
  case TokenType::OR:
    return "||";
  case TokenType::LEFT_SHIFT_ASSIGN:
    return "<<=";
  case TokenType::RIGHT_SHIFT_ASSIGN:
    return ">>=";
  case TokenType::CHARACTER:
    return "CONSTANT";
  case TokenType::BRACE_OPEN_ALT:
    return "<%";
  case TokenType::BRACE_CLOSE_ALT:
    return "%>";
  case TokenType::BRACKET_OPEN_ALT:
    return "<:";
  case TokenType::BRACKET_CLOSE_ALT:
    return ":>";
  case TokenType::HASH:
    return "#";
  case TokenType::HASHHASH:
    return "##";
  case TokenType::HASH_ALT:
    return "%:";
  case TokenType::HASHHASH_ALT:
    return "%:%:";
  case TokenType::FLOAT:
    return "float";
  case TokenType::DOUBLE:
    return "double";
  case TokenType::TOKENEND:
    return "";
  case TokenType::ENDOFFILE:
    return "EOF";
  }
  std::cerr << "error: unknown TokenType";
  return "unknown type";
}

const std::string Token::token_type() const {
  switch (type) {
  case TokenType::NUMBER:
    return "constant";
  case TokenType::IDENTIFIER:
    return "identifier";
  case TokenType::STAR:
    return "punctuator";
  case TokenType::PLUS:
    return "punctuator";
  case TokenType::PLUSPLUS:
    return "punctuator";
  case TokenType::MINUS:
    return "punctuator";
  case TokenType::BRACE_OPEN:
    return "punctuator";
  case TokenType::BRACE_CLOSE:
    return "punctuator";
  case TokenType::AUTO:
    return "keyword";
  case TokenType::BREAK:
    return "keyword";
  case TokenType::CASE:
    return "keyword";
  case TokenType::CHAR:
    return "keyword";
  case TokenType::CONST:
    return "keyword";
  case TokenType::CONTINUE:
    return "keyword";
  case TokenType::DEFAULT:
    return "keyword";
  case TokenType::DO:
    return "keyword";
  case TokenType::ELSE:
    return "keyword";
  case TokenType::ENUM:
    return "keyword";
  case TokenType::EXTERN:
    return "keyword";
  case TokenType::FOR:
    return "keyword";
  case TokenType::GOTO:
    return "keyword";
  case TokenType::IF:
    return "keyword";
  case TokenType::INLINE:
    return "keyword";
  case TokenType::INT:
    return "keyword";
  case TokenType::LONG:
    return "keyword";
  case TokenType::REGISTER:
    return "keyword";
  case TokenType::RESTRICT:
    return "keyword";
  case TokenType::RETURN:
    return "keyword";
  case TokenType::SHORT:
    return "keyword";
  case TokenType::SIGNED:
    return "keyword";
  case TokenType::SIZEOF:
    return "keyword";
  case TokenType::STATIC:
    return "keyword";
  case TokenType::STRUCT:
    return "keyword";
  case TokenType::SWITCH:
    return "keyword";
  case TokenType::TYPEDEF:
    return "keyword";
  case TokenType::UNION:
    return "keyword";
  case TokenType::UNSIGNED:
    return "keyword";
  case TokenType::VOID:
    return "keyword";
  case TokenType::VOLATILE:
    return "keyword";
  case TokenType::WHILE:
    return "keyword";
  case TokenType::ALIGN_AS:
    return "keyword";
  case TokenType::ALIGN_OF:
    return "keyword";
  case TokenType::ATOMIC:
    return "keyword";
  case TokenType::BOOL:
    return "keyword";
  case TokenType::COMPLEX:
    return "keyword";
  case TokenType::GENERIC:
    return "keyword";
  case TokenType::IMAGINARY:
    return "keyword";
  case TokenType::NO_RETURN:
    return "keyword";
  case TokenType::STATIC_ASSERT:
    return "keyword";
  case TokenType::THREAD_LOCAL:
    return "keyword";
  case TokenType::BRACKET_OPEN:
    return "punctuator";
  case TokenType::BRACKET_CLOSE:
    return "punctuator";
  case TokenType::PARENTHESIS_OPEN:
    return "punctuator";
  case TokenType::PARENTHESIS_CLOSE:
    return "punctuator";
  case TokenType::AMPERSAND:
    return "punctuator";
  case TokenType::PIPE:
    return "punctuator";
  case TokenType::CARET:
    return "punctuator";
  case TokenType::TILDE:
    return "punctuator";
  case TokenType::LEFT_SHIFT:
    return "punctuator";
  case TokenType::RIGHT_SHIFT:
    return "punctuator";
  case TokenType::GREATER_EQUAL:
    return "punctuator";
  case TokenType::LESS_EQUAL:
    return "punctuator";
  case TokenType::EQUAL:
    return "punctuator";
  case TokenType::ASSIGN:
    return "punctuator";
  case TokenType::MINUSMINUS:
    return "punctuator";
  case TokenType::DIV:
    return "punctuator";
  case TokenType::MOD:
    return "punctuator";
  case TokenType::PLUS_ASSIGN:
    return "punctuator";
  case TokenType::MINUS_ASSIGN:
    return "punctuator";
  case TokenType::AMPERSAND_ASSIGN:
    return "punctuator";
  case TokenType::PIPE_ASSIGN:
    return "punctuator";
  case TokenType::CARET_ASSIGN:
    return "punctuator";
  case TokenType::STAR_ASSIGN:
    return "punctuator";
  case TokenType::DIV_ASSIGN:
    return "punctuator";
  case TokenType::MOD_ASSIGN:
    return "punctuator";
  case TokenType::STRING:
    return "string-literal";
  case TokenType::LESS:
    return "punctuator";
  case TokenType::GREATER:
    return "punctuator";
  case TokenType::NOT_EQUAL:
    return "punctuator";
  case TokenType::NOT:
    return "punctuator";
  case TokenType::ARROW:
    return "punctuator";
  case TokenType::COMMA:
    return "punctuator";
  case TokenType::COLON:
    return "punctuator";
  case TokenType::QUESTION:
    return "punctuator";
  case TokenType::SEMICOLON:
    return "punctuator";
  case TokenType::TRI_DOTS:
    return "punctuator";
  case TokenType::DOT:
    return "punctuator";
  case TokenType::AND:
    return "punctuator";
  case TokenType::OR:
    return "punctuator";
  case TokenType::LEFT_SHIFT_ASSIGN:
    return "punctuator";
  case TokenType::RIGHT_SHIFT_ASSIGN:
    return "punctuator";
  case TokenType::CHARACTER:
    return "constant";
  case TokenType::BRACE_OPEN_ALT:
    return "punctuator";
  case TokenType::BRACE_CLOSE_ALT:
    return "punctuator";
  case TokenType::BRACKET_OPEN_ALT:
    return "punctuator";
  case TokenType::BRACKET_CLOSE_ALT:
    return "punctuator";
  case TokenType::HASH:
    return "punctuator";
  case TokenType::HASHHASH:
    return "punctuator";
  case TokenType::HASH_ALT:
    return "punctuator";
  case TokenType::HASHHASH_ALT:
    return "punctuator";
  case TokenType::FLOAT:
    return "keyword";
  case TokenType::DOUBLE:
    return "keyword";
  case TokenType::TOKENEND:
    return "";
  case TokenType::ENDOFFILE:
    return "EOF";
  }
  std::cerr << "error: unknown TokenType";
  return "unknown type";
}

unsigned int Token::getPrecedence() {
  switch (type) {
  case TokenType::STAR:
    return 10;
  case TokenType::PLUS:
    return 10;
  case TokenType::MINUS:
    return 10;
  case TokenType::EQUAL:
    return 10;
  case TokenType::NOT_EQUAL:
    return 10;
  case TokenType::AND:
    return 10;
  case TokenType::OR:
    return 10;
  case TokenType::ASSIGN:
    return 10;
  case TokenType::LESS:
    return 10;
  case TokenType::GREATER:
    return 10;
  case TokenType::NOT:
    return 10;
  case TokenType::DOT:
    return 10;
  case TokenType::LEFT_SHIFT_ASSIGN:
    return 10;
  case TokenType::RIGHT_SHIFT_ASSIGN:
    return 10;
  default:
    return 0;
  }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  if (token.extra.empty() && token.getType() != TokenType::STRING) {
    os << token.line << ':' << token.getColumn() << ": " << token.token_type()
       << " " << token.name();
  } else {
    os << token.line << ':' << token.getColumn() << ": " << token.token_type()
       << " ";
    if (token.getType() == TokenType::CHARACTER) {
      os << "'" << token.extra << "'";
    } else if (token.getType() == TokenType::STRING) {
      os << "\"" << token.extra << "\"";
    } else {
      os << token.extra;
    }
  }
  return os;
}
Token::Token(const TokenType type)
    : type(type), line(0), column(0), extra("") {}

} // namespace ccc
