#include <utility>

#include <string>
#include <iostream>
#include "token.hpp"
#include "token_type.hpp"

Token::Token(const TokenType type, const unsigned long line, const unsigned long column, const std::string extra) :
    type(type), line(line), column(column), extra(extra) {}

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
  case TokenType::PLUSPLUS:return "plusplus";
  case TokenType::MINUS:return "minus";
  case TokenType::BRACE_OPEN:return "brace open";
  case TokenType::BRACE_CLOSE:return "brace close";
  case TokenType::LINE_BREAK:return "line break";
  case TokenType::WHITESPACE:return "whitespace";
  case TokenType::AUTO:return "auto";
  case TokenType::BREAK:return "break";
  case TokenType::CASE:return "case";
  case TokenType::CHAR:return "char";
  case TokenType::CONST:return "const";
  case TokenType::CONTINUE:return "continue";
  case TokenType::DEFAULT:return "default";
  case TokenType::DO:return "do";
  case TokenType::ELSE:return "else";
  case TokenType::ENUM:return "enum";
  case TokenType::EXTERN:return "extern";
  case TokenType::FOR:return "for";
  case TokenType::GOTO:return "goto";
  case TokenType::IF:return "if";
  case TokenType::INLINE:return "inline";
  case TokenType::INT:return "int";
  case TokenType::LONG:return "long";
  case TokenType::REGISTER:return "register";
  case TokenType::RESTRICTED:return "restricted";
  case TokenType::RETURN:return "return";
  case TokenType::SHORT:return "short";
  case TokenType::SIGNED:return "signed";
  case TokenType::SIZEOF:return "sizeof";
  case TokenType::STATIC:return "static";
  case TokenType::STRUCT:return "struct";
  case TokenType::SWITCH:return "switch";
  case TokenType::TYPEDEF:return "typedef";
  case TokenType::UNION:return "union";
  case TokenType::UNSIGNED:return "unsigned";
  case TokenType::VOID:return "void";
  case TokenType::VOLATILE:return "volatile";
  case TokenType::WHILE:return "while";
  case TokenType::ALIGN_AS:return "align as";
  case TokenType::ALIGN_OF:return "align of";
  case TokenType::ATOMIC:return "atomic";
  case TokenType::BOOL:return "bool";
  case TokenType::COMPLEX:return "complex";
  case TokenType::GENERIC:return "generic";
  case TokenType::IMAGINARY:return "imaginary";
  case TokenType::NO_RETURN:return "no return";
  case TokenType::STATIC_ASSERT:return "static assert";
  case TokenType::THREAD_LOCAL:return "thread local";
  case TokenType::BRACKET_OPEN:return "bracket open";
  case TokenType::BRACKET_CLOSE:return "bracket close";
  case TokenType::PARENTHESIS_OPEN:return "parenthesis open";
  case TokenType::PARENTHESIS_CLOSE:return "parenthesis close";
  case TokenType::AMPERSAND:return "ampersand";
  case TokenType::PIPE:return "pipe";
  case TokenType::CARET:return "caret";
  case TokenType::TILDE:return "tilde";
  case TokenType::LEFT_SHIFT:return "left shift";
  case TokenType::RIGHT_SHIFT:return "right shift";
  case TokenType::GREATER_EQUAL:return "greater equal";
  case TokenType::LESS_EQUAL:return "less equal";
  case TokenType::EQUAL:return "equal";
  case TokenType::ASSIGN:return "assign";
  case TokenType::MINUSMINUS:return "minus minus";
  case TokenType::DIV:return "div";
  case TokenType::MOD:return "mod";
  case TokenType::PLUS_ASSIGN:return "plus assign";
  case TokenType::MINUS_ASSIGN:return "minus assign";
  case TokenType::AMPERSAND_ASSIGN:return "ampersand assign";
  case TokenType::PIPE_ASSIGN:return "pipe assign";
  case TokenType::CARET_ASSIGN:return "caret assign";
  case TokenType::TILDE_ASSIGN:return "tilde assign";
  case TokenType::STAR_ASSIGN:return "star assign";
  case TokenType::DIV_ASSIGN:return "div assign";
  case TokenType::MOD_ASSIGN:return "mod assign";
  case TokenType::STRING_LITERAL:return "string literal";
  }
  std::cerr << "error: unknown TokenType";
  return "unknown type";
}

const std::string Token::token_type() const {
  switch (type) {
  case TokenType::NUMBER: return "constant";
  case TokenType::IDENTIFIER:return "identifier";
  case TokenType::STAR:return "punctuator";
  case TokenType::PLUS:return "punctuator";
  case TokenType::PLUSPLUS:return "punctuator";
  case TokenType::MINUS:return "punctuator";
  case TokenType::BRACE_OPEN:return "punctuator";
  case TokenType::BRACE_CLOSE:return "punctuator";
  case TokenType::LINE_BREAK:return "whitespace";
  case TokenType::WHITESPACE:return "whitespace";
  case TokenType::AUTO:return "keyword";
  case TokenType::BREAK:return "keyword";
  case TokenType::CASE:return "keyword";
  case TokenType::CHAR:return "keyword";
  case TokenType::CONST:return "keyword";
  case TokenType::CONTINUE:return "keyword";
  case TokenType::DEFAULT:return "keyword";
  case TokenType::DO:return "keyword";
  case TokenType::ELSE:return "keyword";
  case TokenType::ENUM:return "keyword";
  case TokenType::EXTERN:return "keyword";
  case TokenType::FOR:return "keyword";
  case TokenType::GOTO:return "keyword";
  case TokenType::IF:return "keyword";
  case TokenType::INLINE:return "keyword";
  case TokenType::INT:return "keyword";
  case TokenType::LONG:return "keyword";
  case TokenType::REGISTER:return "keyword";
  case TokenType::RESTRICTED:return "keyword";
  case TokenType::RETURN:return "keyword";
  case TokenType::SHORT:return "keyword";
  case TokenType::SIGNED:return "keyword";
  case TokenType::SIZEOF:return "keyword";
  case TokenType::STATIC:return "keyword";
  case TokenType::STRUCT:return "keyword";
  case TokenType::SWITCH:return "keyword";
  case TokenType::TYPEDEF:return "keyword";
  case TokenType::UNION:return "keyword";
  case TokenType::UNSIGNED:return "keyword";
  case TokenType::VOID:return "keyword";
  case TokenType::VOLATILE:return "keyword";
  case TokenType::WHILE:return "keyword";
  case TokenType::ALIGN_AS:return "keyword";
  case TokenType::ALIGN_OF:return "keyword";
  case TokenType::ATOMIC:return "keyword";
  case TokenType::BOOL:return "keyword";
  case TokenType::COMPLEX:return "keyword";
  case TokenType::GENERIC:return "keyword";
  case TokenType::IMAGINARY:return "keyword";
  case TokenType::NO_RETURN:return "nkeyword";
  case TokenType::STATIC_ASSERT:return "keyword";
  case TokenType::THREAD_LOCAL:return "keyword";
  case TokenType::BRACKET_OPEN:return "punctuator";
  case TokenType::BRACKET_CLOSE:return "punctuator";
  case TokenType::PARENTHESIS_OPEN:return "punctuator";
  case TokenType::PARENTHESIS_CLOSE:return "punctuator";
  case TokenType::AMPERSAND:return "punctuator";
  case TokenType::PIPE:return "punctuator";
  case TokenType::CARET:return "punctuator";
  case TokenType::TILDE:return "punctuator";
  case TokenType::LEFT_SHIFT:return "punctuator";
  case TokenType::RIGHT_SHIFT:return "punctuator";
  case TokenType::GREATER_EQUAL:return "punctuator";
  case TokenType::LESS_EQUAL:return "punctuator";
  case TokenType::EQUAL:return "punctuator";
  case TokenType::ASSIGN:return "punctuator";
  case TokenType::MINUSMINUS:return "punctuator";
  case TokenType::DIV:return "punctuator";
  case TokenType::MOD:return "punctuator";
  case TokenType::PLUS_ASSIGN:return "punctuator";
  case TokenType::MINUS_ASSIGN:return "punctuator";
  case TokenType::AMPERSAND_ASSIGN:return "punctuator";
  case TokenType::PIPE_ASSIGN:return "punctuator";
  case TokenType::CARET_ASSIGN:return "punctuator";
  case TokenType::TILDE_ASSIGN:return "punctuator";
  case TokenType::STAR_ASSIGN:return "punctuator";
  case TokenType::DIV_ASSIGN:return "punctuator";
  case TokenType::MOD_ASSIGN:return "mpunctuator";
  case TokenType::STRING_LITERAL:return "string-literal";
  }
  std::cerr << "error: unknown TokenType";
  return "unknown type";
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << token.line << ':' << token.column << ": " << token.token_type() << " " << token.extra;
  return os;
}
