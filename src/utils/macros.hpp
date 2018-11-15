#ifndef C4_ERROR_HPP
#define C4_ERROR_HPP

#define C_TYPES TokenType::VOID, TokenType::CHAR, TokenType::SHORT, \
              TokenType::INT, TokenType::STRUCT \

#define LEXER_ERROR(line, column, msg) std::to_string(line) + ":" + std::to_string(column + 1) + ": error: '" +  msg  +  "'. Lexing Stopped!"
#define PARSER_ERROR(line, column, msg) std::to_string(line) + ":" + std::to_string(column) + ": error: '" +  msg  +  "'. Parsing Stopped!"

#endif

