#ifndef C4_TOKEN_TYPE_HPP
#define C4_TOKEN_TYPE_HPP

#include <string>
enum class TokenType {
  AUTO,
  BREAK,
  CHAR,
  CHARACTER,
  CASE,
  CONST,
  CONTINUE,
  DEFAULT,
  DO,
  ELSE,
  ENUM,
  EXTERN,
  FOR,
  GOTO,
  IF,
  INLINE,
  INT,
  LONG,
  REGISTER,
  RESTRICT,
  RETURN,
  SHORT,
  SIGNED,
  SIZEOF,
  STATIC,
  STRUCT,
  SWITCH,
  TYPEDEF,
  UNION,
  UNSIGNED,
  VOID,
  VOLATILE,
  WHILE,
  ALIGN_AS,
  ALIGN_OF,
  ATOMIC,
  BOOL,
  COMPLEX,
  GENERIC,
  IMAGINARY,
  NO_RETURN,
  STATIC_ASSERT,
  THREAD_LOCAL,
  IDENTIFIER,
  NUMBER,
  STRING,
  BRACE_OPEN,
  BRACE_CLOSE,
  BRACKET_OPEN,
  BRACKET_CLOSE,
  PARENTHESIS_OPEN,
  PARENTHESIS_CLOSE,
  PLUS_ASSIGN,
  PLUSPLUS,
  PLUS,
  MINUS_ASSIGN,
  MINUSMINUS,
  ARROW_STAR,
  ARROW,
  MINUS,
  EQUAL,
  ASSIGN,
  LESS_EQUAL,
  LEFT_SHIFT_ASSIGN,
  LEFT_SHIFT,
  LEFT,
  GREATER_EQUAL,
  RIGHT_SHIFT_ASSIGN,
  RIGHT_SHIFT,
  RIGHT,
  NOT_EQUAL,
  NOT,
  COMMA,
  SEMICOLON,
  TRI_DOTS,
  DOT_STAR,
  DOT,
  CARET_ASSIGN,
  CARET,
  TILDE,
  STAR_ASSIGN,
  STAR,
  DIV_ASSIGN,
  DIV,
  MOD_ASSIGN,
  MOD,
  AMPERSAND_ASSIGN,
  AND,
  AMPERSAND,
  PIPE_ASSIGN,
  OR,
  PIPE,
  COLON_COLON,
  COLON,
  QUESTION,
};

#endif //C4_TOKEN_TYPE_HPP
