#ifndef C4_ERROR_HPP
#define C4_ERROR_HPP

#define C_TYPES                                                                \
  TokenType::VOID, TokenType::CHAR, TokenType::SHORT, TokenType::INT,          \
      TokenType::STRUCT

#define UNARY_OP                                                               \
  TokenType::AMPERSAND, TokenType::STAR, TokenType::PLUS, TokenType::MINUS,    \
      TokenType::NOT, TokenType::SIZEOF

#define BINARY_OP                                                              \
  TokenType::STAR, TokenType::PLUS, TokenType::MINUS, TokenType::LESS,         \
      TokenType::EQUAL, TokenType::NOT_EQUAL, TokenType::AND, TokenType::OR,   \
      TokenType::ASSIGN, TokenType::PLUS_ASSIGN

#define LEXER_ERROR(line, column, msg)                                         \
  std::to_string(line) + ":" + std::to_string(column + 1) + ": error: '" +     \
      msg + "'. Lexing Stopped!"
#define PARSER_ERROR(line, column, msg)                                        \
  std::to_string(line) + ":" + std::to_string(column) + ": error: '" + msg +   \
      "'. Parsing Stopped!"

#define UNUSED(var) (void)var

template <class DstType, class SrcType> bool instanceof (const SrcType *src) {
  return dynamic_cast<const DstType *>(src) != nullptr;
}

#endif
