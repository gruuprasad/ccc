#include "fast_lexer.hpp"
#include <iostream>

namespace ccc {

inline char FastLexer::getCharAt(unsigned long position) {
  return content[position];
}

inline bool FastLexer::keyWordEnd(unsigned long position) {
  const char first = getCharAt(position);
  return !(('0' <= first && first <= '9') || ('a' <= first && first <= 'z') ||
           ('A' <= first && first <= 'Z') || first == '_');
}

inline Token FastLexer::failParsing() {
  std::string msg = "Unknown token " + std::string(1, getCharAt(position));
  if (getCharAt(position + 1) != 0) {
    msg += std::string(1, getCharAt(position + 1));
    if (getCharAt(position + 2) != 0) {
      msg += std::string(1, getCharAt(position + 2));
      if (getCharAt(position + 3) != 0) {
        msg += std::string(1, getCharAt(position + 2)) + " [truncated]";
      }
    }
  }
  error = LEXER_ERROR(line, column, msg);
  return Token(TokenType::INVALIDTOK, line, column);
}

inline Token FastLexer::munchWhitespace() {
  char first = getCharAt(position);
  while (first == ' ' || first == '\t' || first == '\n' || first == '\r') {
    switch (first) {
    case '\r':
      if (getCharAt(position + 1) == '\n') {
        ++position;
      }
      // fall-through
    case '\n':
      column = 0;
      ++line;
      break;
    default:
      ++column;
      break;
    }
    first = getCharAt(++position);
  }
  if (first == 0)
    return Token(TokenType::TOKENEND, line, column);
  return Token(TokenType::WHITESPACE, line, column);
}

inline Token FastLexer::munchLineComment() {
  char first = getCharAt(position);
  while (first != '\n' && first != '\r') {
    first = getCharAt(++position);
    if (first == 0) {
      return Token(TokenType::TOKENEND, line, column);
    }
  }
  switch (first) {
  case '\r':
    if (getCharAt(position + 1) == '\n') {
      ++position;
    }
    // fall-through
  case '\n':
    column = 0;
    ++line;
    break;
  default:
    break;
  }
  ++position;
  return Token(TokenType::LINECOMMENT, line, column);
}

inline Token FastLexer::munchBlockComment() {
  char first = getCharAt(position);
  unsigned long previousLine = line;
  unsigned long previousColumn = column;
  column += 2;
  while (first != '*' || getCharAt(position + 1) != '/') {
    switch (first) {
    case 0:
      error =
          LEXER_ERROR(previousLine, previousColumn, "Unterminated Comment!");
      return Token(TokenType::INVALIDTOK, previousLine, previousColumn);
    case '\r':
      if (getCharAt(position + 1) == '\n') {
        ++position;
      }
      // fall-through
    case '\n':
      column = 0;
      ++line;
      break;
    default:
      ++column;
      break;
    }
    first = getCharAt(++position);
  }
  position += 2;
  column += 2;
  return Token(TokenType::BLOCKCOMMENT, line, column);
}

inline Token FastLexer::munchNumber() {
  unsigned long oldPosition = position;
  char first;
  do {
    first = getCharAt(++position);
  } while ('0' <= first && first <= '9');
  auto result =
      Token(TokenType::NUMBER, line, column,
            std::string(&content[oldPosition], position - oldPosition));
  column += position - oldPosition;
  return result;
}

inline Token FastLexer::munchIdentifier() {
  unsigned long oldPosition = position;
  char first;
  do {
    first = getCharAt(++position);
  } while (('0' <= first && first <= '9') || ('a' <= first && first <= 'z') ||
           ('A' <= first && first <= 'Z') || first == '_');
  auto result =
      Token(TokenType::IDENTIFIER, line, column,
            std::string(&content[oldPosition], position - oldPosition));
  column += position - oldPosition;
  return result;
}

inline Token FastLexer::munchCharacter() {
  Token result;
  char first = getCharAt(++position);
  if (first != '\'' && first != '\\' && first != '\n' && first != '\r' &&
      getCharAt(position + 1) == '\'') {
    auto result =
        Token(TokenType::CHARACTER, line, column, std::string(1, first));
    column += 3;
    position += 2;
    return result;
  }
  if (first == '\\') {
    first = getCharAt(++position);
    switch (first) {
    case '\'':
    case '"':
    case '?':
    case '\\':
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
      result = Token(TokenType::CHARACTER, line, column,
                     std::string(&content[position - 1], 2));
      column += 4;
      position += 2;
      return result;
    default:
      error = LEXER_ERROR(line, column,
                          "Invalid character: '" +
                              std::string(&content[position - 1], 2) + "'");
      return Token(TokenType::INVALIDTOK, line, column, "");
    }
  }
  error = LEXER_ERROR(line, column,
                      "Invalid character: '" + std::string(1, first) + "'");
  return Token(TokenType::INVALIDTOK, line, column, "");
}

inline Token FastLexer::munchString() {
  unsigned long oldPosition = position;
  unsigned long initColumn = column;
  char first = getCharAt(++position);
  ++column;
  while (first != '"') {
    if (first == '\n' || first == '\r' || first == 0) {
      error = LEXER_ERROR(
          line, column,
          "Line break in string at " +
              std::string(&content[oldPosition + 1], position - oldPosition));
      return Token(TokenType::INVALIDTOK, line, column, "");
    }
    if (first == '\\') {
      first = getCharAt(++position);
      ++column;
      switch (first) {
      case '\'':
      case '"':
      case '?':
      case '\\':
      case 'a':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case 'v':
        break;
      default:
        error = LEXER_ERROR(
            line, column,
            "Invalid escape at " +
                std::string(&content[oldPosition + 1], position - oldPosition));
        return Token(TokenType::INVALIDTOK, line, column, "");
      }
    }
    first = getCharAt(++position);
    ++column;
  }
  auto result =
      Token(TokenType::STRING, line, initColumn,
            std::string(&content[oldPosition + 1], position - oldPosition - 1));
  ++position;
  ++column;
  return result;
}

inline Token FastLexer::munchPunctuator() {
  Token result;
  const char first = getCharAt(position);
  switch (first) {
  case '{':
    result = Token(TokenType::BRACE_OPEN, line, column);
    ++position;
    ++column;
    return result;
  case '}':
    result = Token(TokenType::BRACE_CLOSE, line, column);
    ++position;
    ++column;
    return result;
  case '[':
    result = Token(TokenType::BRACKET_OPEN, line, column);
    ++position;
    ++column;
    return result;
  case ']':
    result = Token(TokenType::BRACKET_CLOSE, line, column);
    ++position;
    ++column;
    return result;
  case '(':
    result = Token(TokenType::PARENTHESIS_OPEN, line, column);
    ++position;
    ++column;
    return result;
  case ')':
    result = Token(TokenType::PARENTHESIS_CLOSE, line, column);
    ++position;
    ++column;
    return result;
  case '+':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::PLUS_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    if (getCharAt(position + 1) == '+') {
      result = Token(TokenType::PLUSPLUS, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::PLUS, line, column);
    ++position;
    ++column;
    return result;
  case '-':
    switch (getCharAt(position + 1)) {
    case '-':
      result = Token(TokenType::MINUSMINUS, line, column);
      position += 2;
      column += 2;
      return result;
    case '=':
      result = Token(TokenType::MINUS_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    case '>':
      result = Token(TokenType::ARROW, line, column);
      position += 2;
      column += 2;
      return result;
    default:
      result = Token(TokenType::MINUS, line, column);
      ++position;
      ++column;
      return result;
    }
    break;
  case '=':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::EQUAL, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::ASSIGN, line, column);
    ++position;
    ++column;
    return result;
  case '<':
    switch (getCharAt(position + 1)) {
    case ':':
      result = Token(TokenType::BRACKET_OPEN_ALT, line, column);
      position += 2;
      column += 2;
      return result;
    case '%':
      result = Token(TokenType::BRACE_OPEN_ALT, line, column);
      position += 2;
      column += 2;
      return result;
    case '=':
      result = Token(TokenType::LESS_EQUAL, line, column);
      position += 2;
      column += 2;
      return result;
    case '<':
      if (getCharAt(position + 2) == '=') {
        result = Token(TokenType::LEFT_SHIFT_ASSIGN, line, column);
        position += 3;
        column += 3;
        return result;
      }
      result = Token(TokenType::LEFT_SHIFT, line, column);
      position += 2;
      column += 2;
      return result;
    default:
      result = Token(TokenType::LESS, line, column);
      ++position;
      ++column;
      return result;
    }
    break;
  case '>':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::GREATER_EQUAL, line, column);
      position += 2;
      column += 2;
      return result;
    }
    if (getCharAt(position + 1) == '>') {
      if (getCharAt(position + 2) == '=') {
        result = Token(TokenType::RIGHT_SHIFT_ASSIGN, line, column);
        position += 3;
        column += 3;
        return result;
      }
      result = Token(TokenType::RIGHT_SHIFT, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::GREATER, line, column);
    ++position;
    ++column;
    return result;
  case '!':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::NOT_EQUAL, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::NOT, line, column);
    ++position;
    ++column;
    return result;
  case ',':
    result = Token(TokenType::COMMA, line, column);
    ++position;
    ++column;
    return result;
  case ';':
    result = Token(TokenType::SEMICOLON, line, column);
    ++position;
    ++column;
    return result;
  case '.':
    if (getCharAt(position + 1) == '.' && getCharAt(position + 2) == '.') {
      result = Token(TokenType::TRI_DOTS, line, column);
      position += 3;
      column += 3;
      return result;
    }
    result = Token(TokenType::DOT, line, column);
    ++position;
    ++column;
    return result;
  case '^':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::CARET_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::CARET, line, column);
    ++position;
    ++column;
    return result;
  case '~':
    result = Token(TokenType::TILDE, line, column);
    ++position;
    ++column;
    return result;
  case '*':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::STAR_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::STAR, line, column);
    ++position;
    ++column;
    return result;
  case '/':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::DIV_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::DIV, line, column);
    ++position;
    ++column;
    return result;
  case '%':
    switch (getCharAt(position + 1)) {
    case '=':
      result = Token(TokenType::MOD_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    case ':':
      if (getCharAt(position + 2) == '%' && getCharAt(position + 3) == ':') {
        result = Token(TokenType::HASHHASH_ALT, line, column);
        position += 4;
        column += 4;
        return result;
      }
      result = Token(TokenType::HASH_ALT, line, column);
      position += 2;
      column += 2;
      return result;
    case '>':
      result = Token(TokenType::BRACE_CLOSE_ALT, line, column);
      position += 2;
      column += 2;
      return result;
    default:
      result = Token(TokenType::MOD, line, column);
      ++position;
      ++column;
      return result;
    }
  case '&':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::AMPERSAND_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    if (getCharAt(position + 1) == '&') {
      result = Token(TokenType::AND, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::AMPERSAND, line, column);
    ++position;
    ++column;
    return result;
  case '|':
    if (getCharAt(position + 1) == '=') {
      result = Token(TokenType::PIPE_ASSIGN, line, column);
      position += 2;
      column += 2;
      return result;
    }
    if (getCharAt(position + 1) == '|') {
      result = Token(TokenType::OR, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::PIPE, line, column);
    ++position;
    ++column;
    return result;
  case ':':
    if (getCharAt(position + 1) == '>') {
      result = Token(TokenType::BRACKET_CLOSE_ALT, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::COLON, line, column);
    ++position;
    ++column;
    return result;
  case '#':
    if (getCharAt(position + 1) == '#') {
      result = Token(TokenType::HASHHASH, line, column);
      position += 2;
      column += 2;
      return result;
    }
    result = Token(TokenType::HASH, line, column);
    ++position;
    ++column;
    return result;
  case '?':
    result = Token(TokenType::QUESTION, line, column);
    ++position;
    ++column;
    return result;
  default:
    break;
  }
  /*
   * Fallthrough, no punctuator matched!
   */
  return Token(TokenType::INVALIDTOK, line, column);
}

inline Token FastLexer::munchKeyword() {
  Token result;
  const char first = getCharAt(position);
  switch (first) {
  case 'a':
    if (getCharAt(position + 1) == 'u' && getCharAt(position + 2) == 't' &&
        getCharAt(position + 3) == 'o' && keyWordEnd(position + 4)) {
      result = Token(TokenType::AUTO, line, column);
      position += 4;
      column += 4;
      return result;
    }
    break;
  case 'b':
    if (getCharAt(position + 1) == 'r' && getCharAt(position + 2) == 'e' &&
        getCharAt(position + 3) == 'a' && getCharAt(position + 4) == 'k' &&
        keyWordEnd(position + 5)) {
      result = Token(TokenType::BREAK, line, column);
      position += 5;
      column += 5;
      return result;
    }
    break;
  case 'c':
    switch (getCharAt(position + 1)) {
    case 'a':
      if (getCharAt(position + 2) == 's' && getCharAt(position + 3) == 'e' &&
          keyWordEnd(position + 4)) {
        result = Token(TokenType::CASE, line, column);
        position += 4;
        column += 4;
        return result;
      }
      break;
    case 'h':
      if (getCharAt(position + 2) == 'a' && getCharAt(position + 3) == 'r' &&
          keyWordEnd(position + 4)) {
        result = Token(TokenType::CHAR, line, column);
        position += 4;
        column += 4;
        return result;
      }
      break;
    case 'o':
      if (getCharAt(position + 2) == 'n') {
        if (getCharAt(position + 3) == 's' && getCharAt(position + 4) == 't' &&
            keyWordEnd(position + 5)) {
          result = Token(TokenType::CONST, line, column);
          position += 5;
          column += 5;
          return result;
        }
        if (getCharAt(position + 3) == 't' && getCharAt(position + 4) == 'i' &&
            getCharAt(position + 5) == 'n' && getCharAt(position + 6) == 'u' &&
            getCharAt(position + 7) == 'e' && keyWordEnd(position + 8)) {
          result = Token(TokenType::CONTINUE, line, column);
          position += 8;
          column += 8;
          return result;
        }
      }
      break;
    default:
      break;
    }
    break;
  case 'd':
    if (getCharAt(position + 1) == 'o') {
      if (getCharAt(position + 2) == 'u' && getCharAt(position + 3) == 'b' &&
          getCharAt(position + 4) == 'l' && getCharAt(position + 5) == 'e' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::DOUBLE, line, column);
        position += 6;
        column += 6;
        return result;
      }
      if (keyWordEnd(position + 2)) {
        result = Token(TokenType::DO, line, column);
        position += 2;
        column += 2;
        return result;
      }
      break;
    }
    if (getCharAt(position + 1) == 'e' && getCharAt(position + 2) == 'f' &&
        getCharAt(position + 3) == 'a' && getCharAt(position + 4) == 'u' &&
        getCharAt(position + 5) == 'l' && getCharAt(position + 6) == 't' &&
        keyWordEnd(position + 7)) {
      result = Token(TokenType::DEFAULT, line, column);
      position += 7;
      column += 7;
      return result;
    }
    break;
  case 'e':
    switch (getCharAt(position + 1)) {
    case 'l':
      if (getCharAt(position + 2) == 's' && getCharAt(position + 3) == 'e' &&
          keyWordEnd(position + 4)) {
        result = Token(TokenType::ELSE, line, column);
        position += 4;
        column += 4;
        return result;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 'u' && getCharAt(position + 3) == 'm' &&
          keyWordEnd(position + 4)) {
        result = Token(TokenType::ENUM, line, column);
        position += 4;
        column += 4;
        return result;
      }
      break;
    case 'x':
      if (getCharAt(position + 2) == 't' && getCharAt(position + 3) == 'e' &&
          getCharAt(position + 4) == 'r' && getCharAt(position + 5) == 'n' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::EXTERN, line, column);
        position += 6;
        column += 6;
        return result;
      }
      break;
    default:
      break;
    }
    break;
  case 'f':
    if (getCharAt(position + 1) == 'o' && getCharAt(position + 2) == 'r' &&
        keyWordEnd(position + 3)) {
      result = Token(TokenType::FOR, line, column);
      position += 3;
      column += 3;
      return result;
    }
    if (getCharAt(position + 1) == 'l' && getCharAt(position + 2) == 'o' &&
        getCharAt(position + 3) == 'a' && getCharAt(position + 4) == 't' &&
        keyWordEnd(position + 5)) {
      result = Token(TokenType::FLOAT, line, column);
      position += 5;
      column += 5;
      return result;
    }
    break;
  case 'g':
    if (getCharAt(position + 1) == 'o' && getCharAt(position + 2) == 't' &&
        getCharAt(position + 3) == 'o' && keyWordEnd(position + 4)) {
      result = Token(TokenType::GOTO, line, column);
      position += 4;
      column += 4;
      return result;
    }
    break;
  case 'i':
    switch (getCharAt(position + 1)) {
    case 'f':
      if (keyWordEnd(position + 2)) {
        result = Token(TokenType::IF, line, column);
        position += 2;
        column += 2;
        return result;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 't' && keyWordEnd(position + 3)) {
        result = Token(TokenType::INT, line, column);
        position += 3;
        column += 3;
        return result;
      }
      if (getCharAt(position + 2) == 'l' && getCharAt(position + 3) == 'i' &&
          getCharAt(position + 4) == 'n' && getCharAt(position + 5) == 'e' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::INLINE, line, column);
        position += 6;
        column += 6;
        return result;
      }
      break;
    default:
      break;
    }
    break;
  case 'l':
    if (getCharAt(position + 1) == 'o' && getCharAt(position + 2) == 'n' &&
        getCharAt(position + 3) == 'g' && keyWordEnd(position + 4)) {
      result = Token(TokenType::LONG, line, column);
      position += 4;
      column += 4;
      return result;
    }
    break;
  case 'r':
    if (getCharAt(position + 1) == 'e') {
      switch (getCharAt(position + 2)) {
      case 'g':
        if (getCharAt(position + 3) == 'i' && getCharAt(position + 4) == 's' &&
            getCharAt(position + 5) == 't' && getCharAt(position + 6) == 'e' &&
            getCharAt(position + 7) == 'r' && keyWordEnd(position + 8)) {
          result = Token(TokenType::REGISTER, line, column);
          position += 8;
          column += 8;
          return result;
        }
        break;
      case 's':
        if (getCharAt(position + 3) == 't' && getCharAt(position + 4) == 'r' &&
            getCharAt(position + 5) == 'i' && getCharAt(position + 6) == 'c' &&
            getCharAt(position + 7) == 't' && keyWordEnd(position + 8)) {
          result = Token(TokenType::RESTRICT, line, column);
          position += 8;
          column += 8;
          return result;
        }
        break;
      case 't':
        if (getCharAt(position + 3) == 'u' && getCharAt(position + 4) == 'r' &&
            getCharAt(position + 5) == 'n' && keyWordEnd(position + 6)) {
          result = Token(TokenType::RETURN, line, column);
          position += 6;
          column += 6;
          return result;
        }
        break;
      default:
        break;
      }
      break;
    }
    break;
  case 's':
    switch (getCharAt(position + 1)) {
    case 'h':
      if (getCharAt(position + 2) == 'o' && getCharAt(position + 3) == 'r' &&
          getCharAt(position + 4) == 't' && keyWordEnd(position + 5)) {
        result = Token(TokenType::SHORT, line, column);
        position += 5;
        column += 5;
        return result;
      }
      break;
    case 'i':
      if (getCharAt(position + 2) == 'g' && getCharAt(position + 3) == 'n' &&
          getCharAt(position + 4) == 'e' && getCharAt(position + 5) == 'd' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::SIGNED, line, column);
        position += 6;
        column += 6;
        return result;
      }
      if (getCharAt(position + 2) == 'z' && getCharAt(position + 3) == 'e' &&
          getCharAt(position + 4) == 'o' && getCharAt(position + 5) == 'f' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::SIZEOF, line, column);
        position += 6;
        column += 6;
        return result;
      }
      break;
    case 't':
      if (getCharAt(position + 2) == 'a' && getCharAt(position + 3) == 't' &&
          getCharAt(position + 4) == 'i' && getCharAt(position + 5) == 'c' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::STATIC, line, column);
        position += 6;
        column += 6;
        return result;
      }
      if (getCharAt(position + 2) == 'r' && getCharAt(position + 3) == 'u' &&
          getCharAt(position + 4) == 'c' && getCharAt(position + 5) == 't' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::STRUCT, line, column);
        position += 6;
        column += 6;
        return result;
      }
      break;
    case 'w':
      if (getCharAt(position + 2) == 'i' && getCharAt(position + 3) == 't' &&
          getCharAt(position + 4) == 'c' && getCharAt(position + 5) == 'h' &&
          keyWordEnd(position + 6)) {
        result = Token(TokenType::SWITCH, line, column);
        position += 6;
        column += 6;
        return result;
      }
      break;
    default:
      break;
    }
    break;
  case 't':
    if (getCharAt(position + 1) == 'y' && getCharAt(position + 2) == 'p' &&
        getCharAt(position + 3) == 'e' && getCharAt(position + 4) == 'd' &&
        getCharAt(position + 5) == 'e' && getCharAt(position + 6) == 'f' &&
        keyWordEnd(position + 7)) {
      result = Token(TokenType::TYPEDEF, line, column);
      position += 7;
      column += 7;
      return result;
    }
    break;
  case 'u':
    if (getCharAt(position + 1) == 'n') {
      if (getCharAt(position + 2) == 'i' && getCharAt(position + 3) == 'o' &&
          getCharAt(position + 4) == 'n' && keyWordEnd(position + 5)) {
        result = Token(TokenType::UNION, line, column);
        position += 5;
        column += 5;
        return result;
      }
      if (getCharAt(position + 2) == 's' && getCharAt(position + 3) == 'i' &&
          getCharAt(position + 4) == 'g' && getCharAt(position + 5) == 'n' &&
          getCharAt(position + 6) == 'e' && getCharAt(position + 7) == 'd' &&
          keyWordEnd(position + 8)) {
        result = Token(TokenType::UNSIGNED, line, column);
        position += 8;
        column += 8;
        return result;
      }
    }
    break;
  case 'v':
    if (getCharAt(position + 1) == 'o') {
      if (getCharAt(position + 2) == 'i' && getCharAt(position + 3) == 'd' &&
          keyWordEnd(position + 4)) {
        result = Token(TokenType::VOID, line, column);
        position += 4;
        column += 4;
        return result;
      }
      if (getCharAt(position + 2) == 'l' && getCharAt(position + 3) == 'a' &&
          getCharAt(position + 4) == 't' && getCharAt(position + 5) == 'i' &&
          getCharAt(position + 6) == 'l' && getCharAt(position + 7) == 'e' &&
          keyWordEnd(position + 8)) {
        result = Token(TokenType::VOLATILE, line, column);
        position += 8;
        column += 8;
        return result;
      }
    }
    break;
  case 'w':
    if (getCharAt(position + 1) == 'h' && getCharAt(position + 2) == 'i' &&
        getCharAt(position + 3) == 'l' && getCharAt(position + 4) == 'e' &&
        keyWordEnd(position + 5)) {
      result = Token(TokenType::WHILE, line, column);
      position += 5;
      column += 5;
      return result;
    }
    break;
  case '_':
    switch (getCharAt(position + 1)) {
    case 'A':
      if (getCharAt(position + 2) == 'l' && getCharAt(position + 3) == 'i' &&
          getCharAt(position + 4) == 'g' && getCharAt(position + 5) == 'n') {
        if (getCharAt(position + 6) == 'a' && getCharAt(position + 7) == 's' &&
            keyWordEnd(position + 8)) {
          result = Token(TokenType::ALIGN_AS, line, column);
          position += 8;
          column += 8;
          return result;
        }
        if (getCharAt(position + 6) == 'o' && getCharAt(position + 7) == 'f' &&
            keyWordEnd(position + 8)) {
          result = Token(TokenType::ALIGN_OF, line, column);
          position += 8;
          column += 8;
          return result;
        }
      }
      if (getCharAt(position + 2) == 't' && getCharAt(position + 3) == 'o' &&
          getCharAt(position + 4) == 'm' && getCharAt(position + 5) == 'i' &&
          getCharAt(position + 6) == 'c' && keyWordEnd(position + 7)) {
        result = Token(TokenType::ATOMIC, line, column);
        position += 7;
        column += 7;
        return result;
      }
      break;
    case 'B':
      if (getCharAt(position + 2) == 'o' && getCharAt(position + 3) == 'o' &&
          getCharAt(position + 4) == 'l' && keyWordEnd(position + 5)) {
        result = Token(TokenType::BOOL, line, column);
        position += 5;
        column += 5;
        return result;
      }
      break;
    case 'C':
      if (getCharAt(position + 2) == 'o' && getCharAt(position + 3) == 'm' &&
          getCharAt(position + 4) == 'p' && getCharAt(position + 5) == 'l' &&
          getCharAt(position + 6) == 'e' && getCharAt(position + 7) == 'x' &&
          keyWordEnd(position + 8)) {
        result = Token(TokenType::COMPLEX, line, column);
        position += 8;
        column += 8;
        return result;
      }
      break;
    case 'G':
      if (getCharAt(position + 2) == 'e' && getCharAt(position + 3) == 'n' &&
          getCharAt(position + 4) == 'e' && getCharAt(position + 5) == 'r' &&
          getCharAt(position + 6) == 'i' && getCharAt(position + 7) == 'c' &&
          keyWordEnd(position + 8)) {
        result = Token(TokenType::GENERIC, line, column);
        position += 8;
        column += 8;
        return result;
      }
      break;
    case 'I':
      if (getCharAt(position + 2) == 'm' && getCharAt(position + 3) == 'a' &&
          getCharAt(position + 4) == 'g' && getCharAt(position + 5) == 'i' &&
          getCharAt(position + 6) == 'n' && getCharAt(position + 7) == 'a' &&
          getCharAt(position + 8) == 'r' && getCharAt(position + 9) == 'y' &&
          keyWordEnd(position + 10)) {
        result = Token(TokenType::IMAGINARY, line, column);
        position += 10;
        column += 10;
        return result;
      }
      break;
    case 'N':
      if (getCharAt(position + 2) == 'o' && getCharAt(position + 3) == 'r' &&
          getCharAt(position + 4) == 'e' && getCharAt(position + 5) == 't' &&
          getCharAt(position + 6) == 'u' && getCharAt(position + 7) == 'r' &&
          getCharAt(position + 8) == 'n' && keyWordEnd(position + 9)) {
        result = Token(TokenType::NO_RETURN, line, column);
        position += 9;
        column += 9;
        return result;
      }
      break;
    case 'S':
      if (getCharAt(position + 2) == 't' && getCharAt(position + 3) == 'a' &&
          getCharAt(position + 4) == 't' && getCharAt(position + 5) == 'i' &&
          getCharAt(position + 6) == 'c' && getCharAt(position + 7) == '_' &&
          getCharAt(position + 8) == 'a' && getCharAt(position + 9) == 's' &&
          getCharAt(position + 10) == 's' && getCharAt(position + 11) == 'e' &&
          getCharAt(position + 12) == 'r' && getCharAt(position + 13) == 't' &&
          keyWordEnd(position + 14)) {
        result = Token(TokenType::STATIC_ASSERT, line, column);
        position += 14;
        column += 14;
        return result;
      }
      break;
    case 'T':
      if (getCharAt(position + 2) == 'h' && getCharAt(position + 3) == 'r' &&
          getCharAt(position + 4) == 'e' && getCharAt(position + 5) == 'a' &&
          getCharAt(position + 6) == 'd' && getCharAt(position + 7) == '_' &&
          getCharAt(position + 8) == 'l' && getCharAt(position + 9) == 'o' &&
          getCharAt(position + 10) == 'c' && getCharAt(position + 11) == 'a' &&
          getCharAt(position + 12) == 'l' && keyWordEnd(position + 13)) {
        result = Token(TokenType::THREAD_LOCAL, line, column);
        position += 13;
        column += 13;
        return result;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  /*
   * Fallthrough, no keyword matched!
   */
  return Token(TokenType::NONKEYWORD, line, column);
}

inline Token FastLexer::munch() {
  Token result;
  switch (getCharAt(position)) {
  case 0:
    return Token(TokenType::TOKENEND, line, column);
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return munchWhitespace();
  case '/':
    if (getCharAt(position + 1) == '/') {
      position += 2;
      return munchLineComment();
    }
    if (getCharAt(position + 1) == '*') {
      position += 2;
      return munchBlockComment();
    }
    // fall-through
  case '{':
  case '}':
  case '[':
  case ']':
  case '(':
  case ')':
  case '+':
  case '-':
  case '=':
  case '<':
  case '>':
  case '!':
  case ',':
  case ';':
  case '.':
  case '^':
  case '~':
  case '*':
  case '%':
  case '&':
  case '|':
  case ':':
  case '#':
  case '?':
    result = munchPunctuator();
    if (result.getType() == TokenType::INVALIDTOK)
      return failParsing();
    return result;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return munchNumber();
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'i':
  case 'l':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case '_':
    result = munchKeyword();
    if (result.getType() != TokenType::NONKEYWORD)
      return result;
    // fall-through
  case 'h':
  case 'j':
  case 'k':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
    return munchIdentifier();
  case '\'':
    return munchCharacter();
  case '"':
    return munchString();
  default:
    return failParsing();
  }
}

Token FastLexer::lex_valid() {
  while (true) {
    auto curToken = munch();
    switch (curToken.getType()) {
    case TokenType::BLOCKCOMMENT:
    case TokenType::LINECOMMENT:
    case TokenType::WHITESPACE:
      // These tokens are skipped over.
      continue;
    default:
      return curToken;
    }
  };
}

std::vector<Token> FastLexer::lex() {
  std::vector<Token> token_list;
  token_list.reserve(content.size());
  Token curToken;
  while (true) {
    curToken = lex_valid();
    if (curToken.getType() == TokenType::INVALIDTOK ||
        curToken.getType() == TokenType::TOKENEND) {
      break;
    }
    token_list.push_back(std::move(curToken));
  }
  return token_list;
}

} // namespace ccc
