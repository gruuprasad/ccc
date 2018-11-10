#include <iostream>
#include "lexer_exception.hpp"
#include "fast_lexer.hpp"

#define ERROR_STR(line, column, msg) std::to_string(line) + ":" + std::to_string(column + 1) + ": error: '" +  msg  +  "'. Lexing Stopped!"

namespace ccc {

FastLexer::FastLexer(const std::string &content) : content(content) {
  token_list.reserve(content.size());
}

inline char FastLexer::getCharAt(unsigned long position) {
  return content[position];
}

inline bool FastLexer::keyWordEnd(unsigned long position) {
  const char first = getCharAt(position);
  return !(('0' <= first && first <= '9')
      || ('a' <= first && first <= 'z')
      || ('A' <= first && first <= 'Z')
      || first == '_');
}

inline bool FastLexer::failParsing() {
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
  error = ERROR_STR(line, column, msg);
  return false;
}

inline bool FastLexer::munchWhitespace() {
  char first = getCharAt(position);
  while (first == ' '
      || first == '\t'
      || first == '\n'
      || first == '\r'
      ) {
    switch (first) {
    case '\r':
      if (getCharAt(position + 1) == '\n') {
        ++position;
      }
      //fall-through
    case '\n':column = 0;
      ++line;
      break;
    default:++column;
      break;
    }
    first = getCharAt(++position);
  }
  return first != 0;
}

inline bool FastLexer::munchLineComment() {
  char first = getCharAt(position);
  while (first != '\n' && first != '\r') {
    first = getCharAt(++position);
    if (first == 0) {
      return false;
    }
  }
  switch (first) {
  case '\r':
    if (getCharAt(position + 1) == '\n') {
      ++position;
    }
    //fall-through
  case '\n':column = 0;
    ++line;
    break;
  default:break;
  }
  ++position;
  return true;
}

inline bool FastLexer::munchBlockComment() {
  char first = getCharAt(position);
  unsigned long previousLine = line;
  unsigned long previousColumn = column;
  column += 2;
  while (first != '*' || getCharAt(position + 1) != '/') {
    switch (first) {
    case 0:
      error = ERROR_STR(previousLine, previousColumn, "Unterminated Comment!");
      return false;
    case '\r':
      if (getCharAt(position + 1) == '\n') {
        ++position;
      }
      //fall-through
    case '\n':column = 0;
      ++line;
      break;
    default:++column;
      break;
    }
    first = getCharAt(++position);
  }
  position += 2;
  column += 2;
  return true;
}

inline bool FastLexer::munchNumber() {
  unsigned long oldPosition = position;
  char first;
  do {
    first = getCharAt(++position);
  } while ('0' <= first && first <= '9');
  token_list.emplace_back(TokenType::NUMBER, line, column,
                                std::string(&content[oldPosition], position - oldPosition));
  column += position - oldPosition;
  return true;
}

inline bool FastLexer::munchIdentifier() {
  unsigned long oldPosition = position;
  char first;
  do {
    first = getCharAt(++position);
  } while (('0' <= first && first <= '9')
      || ('a' <= first && first <= 'z')
      || ('A' <= first && first <= 'Z')
      || first == '_');
  token_list.emplace_back(TokenType::IDENTIFIER,
                                line,
                                column,
                                std::string(&content[oldPosition], position - oldPosition));
  column += position - oldPosition;
  return true;
}

inline bool FastLexer::munchCharacter() {
  char first = getCharAt(++position);
  if (first != '\''
      && first != '\\'
      && first != '\n'
      && first != '\r'
      && getCharAt(position + 1) == '\'') {
    token_list.emplace_back(TokenType::CHARACTER, line, column, std::string(1, first));
    column += 3;
    position += 2;
    return true;
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
      token_list.emplace_back(TokenType::CHARACTER,
                                    line,
                                    column,
                                    std::string(&content[position - 1], 2));
      column += 4;
      position += 2;
      return true;
    default:
      error = ERROR_STR(line, column, "Invalid character: '" + std::string(&content[position - 1], 2) + "'");
      return false;
    }
  }
  error = ERROR_STR(line, column, "Invalid character: '" + std::string(1, first) + "'");
  return false;
}

inline bool FastLexer::munchString() {
  unsigned long oldPosition = position;
  unsigned long initColumn = column;
  char first = getCharAt(++position);
  ++column;
  while (first != '"') {
    if (first == '\n'
        || first == '\r'
        || first == 0) {
      error = ERROR_STR(line, column, "Line break in string at " + std::string(&content[oldPosition + 1], position - oldPosition));
      return false;
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
      case 'v':break;
      default:
        error = ERROR_STR(line, column, "Invalid escape at " + std::string(&content[oldPosition + 1], position - oldPosition));
        return false;
      }
    }
    first = getCharAt(++position);
    ++column;
  }
  token_list.emplace_back(TokenType::STRING,
                                line,
                                initColumn,
                                std::string(&content[oldPosition + 1], position - oldPosition - 1));
  ++position;
  ++column;
  return true;
}

inline bool FastLexer::isPunctuator() {
  const char first = getCharAt(position);
  switch (first) {
  case '{':token_list.emplace_back(TokenType::BRACE_OPEN, line, column);
    ++position;
    ++column;
    return true;
  case '}':token_list.emplace_back(TokenType::BRACE_CLOSE, line, column);
    ++position;
    ++column;
    return true;
  case '[':token_list.emplace_back(TokenType::BRACKET_OPEN, line, column);
    ++position;
    ++column;
    return true;
  case ']':token_list.emplace_back(TokenType::BRACKET_CLOSE, line, column);
    ++position;
    ++column;
    return true;
  case '(':token_list.emplace_back(TokenType::PARENTHESIS_OPEN, line, column);
    ++position;
    ++column;
    return true;
  case ')':token_list.emplace_back(TokenType::PARENTHESIS_CLOSE, line, column);
    ++position;
    ++column;
    return true;
  case '+':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::PLUS_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '+') {
      token_list.emplace_back(TokenType::PLUSPLUS, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::PLUS, line, column);
    ++position;
    ++column;
    return true;
  case '-':
    switch (getCharAt(position + 1)) {
    case '-':token_list.emplace_back(TokenType::MINUSMINUS, line, column);
      position += 2;
      column += 2;
      return true;
    case '=':token_list.emplace_back(TokenType::MINUS_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    case '>':token_list.emplace_back(TokenType::ARROW, line, column);
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(TokenType::MINUS, line, column);
      ++position;
      ++column;
      return true;
    }
    break;
  case '=':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::EQUAL, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::ASSIGN, line, column);
    ++position;
    ++column;
    return true;
  case '<':
    switch (getCharAt(position + 1)) {
    case ':':token_list.emplace_back(TokenType::BRACKET_OPEN_ALT, line, column);
      position += 2;
      column += 2;
      return true;
    case '%':token_list.emplace_back(TokenType::BRACE_OPEN_ALT, line, column);
      position += 2;
      column += 2;
      return true;
    case '=':token_list.emplace_back(TokenType::LESS_EQUAL, line, column);
      position += 2;
      column += 2;
      return true;
    case '<':
      if (getCharAt(position + 2) == '=') {
        token_list.emplace_back(TokenType::LEFT_SHIFT_ASSIGN, line, column);
        position += 3;
        column += 3;
        return true;
      }
      token_list.emplace_back(TokenType::LEFT_SHIFT, line, column);
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(TokenType::LESS, line, column);
      ++position;
      ++column;
      return true;
    }
    break;
  case '>':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::GREATER_EQUAL, line, column);
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '>') {
      if (getCharAt(position + 2) == '=') {
        token_list.emplace_back(TokenType::RIGHT_SHIFT_ASSIGN, line, column);
        position += 3;
        column += 3;
        return true;
      }
      token_list.emplace_back(TokenType::RIGHT_SHIFT, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::GREATER, line, column);
    ++position;
    ++column;
    return true;
  case '!':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::NOT_EQUAL, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::NOT, line, column);
    ++position;
    ++column;
    return true;
  case ',':token_list.emplace_back(TokenType::COMMA, line, column);
    ++position;
    ++column;
    return true;
  case ';':token_list.emplace_back(TokenType::SEMICOLON, line, column);
    ++position;
    ++column;
    return true;
  case '.':
    if (getCharAt(position + 1) == '.'
        && getCharAt(position + 2) == '.') {
      token_list.emplace_back(TokenType::TRI_DOTS, line, column);
      position += 3;
      column += 3;
      return true;
    }
    token_list.emplace_back(TokenType::DOT, line, column);
    ++position;
    ++column;
    return true;
  case '^':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::CARET_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::CARET, line, column);
    ++position;
    ++column;
    return true;
  case '~':token_list.emplace_back(TokenType::TILDE, line, column);
    ++position;
    ++column;
    return true;
  case '*':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::STAR_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::STAR, line, column);
    ++position;
    ++column;
    return true;
  case '/':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::DIV_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::DIV, line, column);
    ++position;
    ++column;
    return true;
  case '%':
    switch (getCharAt(position + 1)) {
    case '=':token_list.emplace_back(TokenType::MOD_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    case ':':
      if (getCharAt(position + 2) == '%'
          && getCharAt(position + 3) == ':') {
        token_list.emplace_back(TokenType::HASHHASH_ALT, line, column);
        position += 4;
        column += 4;
        return true;
      }
      token_list.emplace_back(TokenType::HASH_ALT, line, column);
      position += 2;
      column += 2;
      return true;
    case '>':token_list.emplace_back(TokenType::BRACE_CLOSE_ALT, line, column);
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(TokenType::MOD, line, column);
      ++position;
      ++column;
      return true;
    }
  case '&':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::AMPERSAND_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '&') {
      token_list.emplace_back(TokenType::AND, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::AMPERSAND, line, column);
    ++position;
    ++column;
    return true;
  case '|':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(TokenType::PIPE_ASSIGN, line, column);
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '|') {
      token_list.emplace_back(TokenType::OR, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::PIPE, line, column);
    ++position;
    ++column;
    return true;
  case ':':
    if (getCharAt(position + 1) == '>') {
      token_list.emplace_back(TokenType::BRACKET_CLOSE_ALT, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::COLON, line, column);
    ++position;
    ++column;
    return true;
  case '#':
    if (getCharAt(position + 1) == '#') {
      token_list.emplace_back(TokenType::HASHHASH, line, column);
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(TokenType::HASH, line, column);
    ++position;
    ++column;
    return true;
  case '?':token_list.emplace_back(TokenType::QUESTION, line, column);
    ++position;
    ++column;
    return true;
  default: break;
  }
  /*
   * Fallthrough, no punctuator matched!
   */
  return false;
}

inline bool FastLexer::isKeyword() {
  const char first = getCharAt(position);
  switch (first) {
  case 'a':
    if (getCharAt(position + 1) == 'u'
        && getCharAt(position + 2) == 't'
        && getCharAt(position + 3) == 'o'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(TokenType::AUTO, line, column);
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'b':
    if (getCharAt(position + 1) == 'r'
        && getCharAt(position + 2) == 'e'
        && getCharAt(position + 3) == 'a'
        && getCharAt(position + 4) == 'k'
        && keyWordEnd(position + 5)
        ) {
      token_list.emplace_back(TokenType::BREAK, line, column);
      position += 5;
      column += 5;
      return true;
    }
    break;
  case 'c':
    switch (getCharAt(position + 1)) {
    case 'a':
      if (getCharAt(position + 2) == 's'
          && getCharAt(position + 3) == 'e'
          && keyWordEnd(position + 4)
          ) {
        token_list.emplace_back(TokenType::CASE, line, column);
        position += 4;
        column += 4;
        return true;
      }
      break;
    case 'h':
      if (getCharAt(position + 2) == 'a'
          && getCharAt(position + 3) == 'r'
          && keyWordEnd(position + 4)
          ) {
        token_list.emplace_back(TokenType::CHAR, line, column);
        position += 4;
        column += 4;
        return true;
      }
      break;
    case 'o':
      if (getCharAt(position + 2) == 'n') {
        if (getCharAt(position + 3) == 's'
            && getCharAt(position + 4) == 't'
            && keyWordEnd(position + 5)
            ) {
          token_list.emplace_back(TokenType::CONST, line, column);
          position += 5;
          column += 5;
          return true;
        }
        if (getCharAt(position + 3) == 't'
            && getCharAt(position + 4) == 'i'
            && getCharAt(position + 5) == 'n'
            && getCharAt(position + 6) == 'u'
            && getCharAt(position + 7) == 'e'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(TokenType::CONTINUE, line, column);
          position += 8;
          column += 8;
          return true;
        }
      }
      break;
    default:break;
    }
    break;
  case 'd':
    if (getCharAt(position + 1) == 'o') {
      if (getCharAt(position + 2) == 'u'
          && getCharAt(position + 3) == 'b'
          && getCharAt(position + 4) == 'l'
          && getCharAt(position + 5) == 'e'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::DOUBLE, line, column);
        position += 6;
        column += 6;
        return true;
      }
      if (keyWordEnd(position + 2)) {
        token_list.emplace_back(TokenType::DO, line, column);
        position += 2;
        column += 2;
        return true;
      }
      break;
    }
    if (getCharAt(position + 1) == 'e'
        && getCharAt(position + 2) == 'f'
        && getCharAt(position + 3) == 'a'
        && getCharAt(position + 4) == 'u'
        && getCharAt(position + 5) == 'l'
        && getCharAt(position + 6) == 't'
        && keyWordEnd(position + 7)
        ) {
      token_list.emplace_back(TokenType::DEFAULT, line, column);
      position += 7;
      column += 7;
      return true;
    }
    break;
  case 'e':
    switch (getCharAt(position + 1)) {
    case 'l':
      if (getCharAt(position + 2) == 's'
          && getCharAt(position + 3) == 'e'
          && keyWordEnd(position + 4)
          ) {
        token_list.emplace_back(TokenType::ELSE, line, column);
        position += 4;
        column += 4;
        return true;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 'u'
          && getCharAt(position + 3) == 'm'
          && keyWordEnd(position + 4)
          ) {
        token_list.emplace_back(TokenType::ENUM, line, column);
        position += 4;
        column += 4;
        return true;
      }
      break;
    case 'x':
      if (getCharAt(position + 2) == 't'
          && getCharAt(position + 3) == 'e'
          && getCharAt(position + 4) == 'r'
          && getCharAt(position + 5) == 'n'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::EXTERN, line, column);
        position += 6;
        column += 6;
        return true;
      }
      break;
    default:break;
    }
    break;
  case 'f':
    if (getCharAt(position + 1) == 'o'
        && getCharAt(position + 2) == 'r'
        && keyWordEnd(position + 3)
        ) {
      token_list.emplace_back(TokenType::FOR, line, column);
      position += 3;
      column += 3;
      return true;
    }
    if (getCharAt(position + 1) == 'l'
        && getCharAt(position + 2) == 'o'
        && getCharAt(position + 3) == 'a'
        && getCharAt(position + 4) == 't'
        && keyWordEnd(position + 5)
        ) {
      token_list.emplace_back(TokenType::FLOAT, line, column);
      position += 5;
      column += 5;
      return true;
    }
    break;
  case 'g':
    if (getCharAt(position + 1) == 'o'
        && getCharAt(position + 2) == 't'
        && getCharAt(position + 3) == 'o'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(TokenType::GOTO, line, column);
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'i':
    switch (getCharAt(position + 1)) {
    case 'f':
      if (keyWordEnd(position + 2)) {
        token_list.emplace_back(TokenType::IF, line, column);
        position += 2;
        column += 2;
        return true;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 't'
          && keyWordEnd(position + 3)
          ) {
        token_list.emplace_back(TokenType::INT, line, column);
        position += 3;
        column += 3;
        return true;
      }
      if (getCharAt(position + 2) == 'l'
          && getCharAt(position + 3) == 'i'
          && getCharAt(position + 4) == 'n'
          && getCharAt(position + 5) == 'e'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::INLINE, line, column);
        position += 6;
        column += 6;
        return true;
      }
      break;
    default:break;
    }
    break;
  case 'l':
    if (getCharAt(position + 1) == 'o'
        && getCharAt(position + 2) == 'n'
        && getCharAt(position + 3) == 'g'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(TokenType::LONG, line, column);
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'r':
    if (getCharAt(position + 1) == 'e') {
      switch (getCharAt(position + 2)) {
      case 'g':
        if (getCharAt(position + 3) == 'i'
            && getCharAt(position + 4) == 's'
            && getCharAt(position + 5) == 't'
            && getCharAt(position + 6) == 'e'
            && getCharAt(position + 7) == 'r'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(TokenType::REGISTER, line, column);
          position += 8;
          column += 8;
          return true;
        }
        break;
      case 's':
        if (getCharAt(position + 3) == 't'
            && getCharAt(position + 4) == 'r'
            && getCharAt(position + 5) == 'i'
            && getCharAt(position + 6) == 'c'
            && getCharAt(position + 7) == 't'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(TokenType::RESTRICT, line, column);
          position += 8;
          column += 8;
          return true;
        }
        break;
      case 't':
        if (getCharAt(position + 3) == 'u'
            && getCharAt(position + 4) == 'r'
            && getCharAt(position + 5) == 'n'
            && keyWordEnd(position + 6)
            ) {
          token_list.emplace_back(TokenType::RETURN, line, column);
          position += 6;
          column += 6;
          return true;
        }
        break;
      default:break;
      }
      break;
    }
    break;
  case 's':
    switch (getCharAt(position + 1)) {
    case 'h':
      if (getCharAt(position + 2) == 'o'
          && getCharAt(position + 3) == 'r'
          && getCharAt(position + 4) == 't'
          && keyWordEnd(position + 5)
          ) {
        token_list.emplace_back(TokenType::SHORT, line, column);
        position += 5;
        column += 5;
        return true;
      }
      break;
    case 'i':
      if (getCharAt(position + 2) == 'g'
          && getCharAt(position + 3) == 'n'
          && getCharAt(position + 4) == 'e'
          && getCharAt(position + 5) == 'd'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::SIGNED, line, column);
        position += 6;
        column += 6;
        return true;
      }
      if (getCharAt(position + 2) == 'z'
          && getCharAt(position + 3) == 'e'
          && getCharAt(position + 4) == 'o'
          && getCharAt(position + 5) == 'f'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::SIZEOF, line, column);
        position += 6;
        column += 6;
        return true;
      }
      break;
    case 't':
      if (getCharAt(position + 2) == 'a'
          && getCharAt(position + 3) == 't'
          && getCharAt(position + 4) == 'i'
          && getCharAt(position + 5) == 'c'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::STATIC, line, column);
        position += 6;
        column += 6;
        return true;
      }
      if (getCharAt(position + 2) == 'r'
          && getCharAt(position + 3) == 'u'
          && getCharAt(position + 4) == 'c'
          && getCharAt(position + 5) == 't'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::STRUCT, line, column);
        position += 6;
        column += 6;
        return true;
      }
      break;
    case 'w':
      if (getCharAt(position + 2) == 'i'
          && getCharAt(position + 3) == 't'
          && getCharAt(position + 4) == 'c'
          && getCharAt(position + 5) == 'h'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(TokenType::SWITCH, line, column);
        position += 6;
        column += 6;
        return true;
      }
      break;
    default:break;
    }
    break;
  case 't':
    if (getCharAt(position + 1) == 'y'
        && getCharAt(position + 2) == 'p'
        && getCharAt(position + 3) == 'e'
        && getCharAt(position + 4) == 'd'
        && getCharAt(position + 5) == 'e'
        && getCharAt(position + 6) == 'f'
        && keyWordEnd(position + 7)
        ) {
      token_list.emplace_back(TokenType::TYPEDEF, line, column);
      position += 7;
      column += 7;
      return true;
    }
    break;
  case 'u':
    if (getCharAt(position + 1) == 'n') {
      if (getCharAt(position + 2) == 'i'
          && getCharAt(position + 3) == 'o'
          && getCharAt(position + 4) == 'n'
          && keyWordEnd(position + 5)
          ) {
        token_list.emplace_back(TokenType::UNION, line, column);
        position += 5;
        column += 5;
        return true;
      }
      if (getCharAt(position + 2) == 's'
          && getCharAt(position + 3) == 'i'
          && getCharAt(position + 4) == 'g'
          && getCharAt(position + 5) == 'n'
          && getCharAt(position + 6) == 'e'
          && getCharAt(position + 7) == 'd'
          && keyWordEnd(position + 8)
          ) {
        token_list.emplace_back(TokenType::UNSIGNED, line, column);
        position += 8;
        column += 8;
        return true;
      }
    }
    break;
  case 'v':
    if (getCharAt(position + 1) == 'o') {
      if (getCharAt(position + 2) == 'i'
          && getCharAt(position + 3) == 'd'
          && keyWordEnd(position + 4)
          ) {
        token_list.emplace_back(TokenType::VOID, line, column);
        position += 4;
        column += 4;
        return true;
      }
      if (getCharAt(position + 2) == 'l'
          && getCharAt(position + 3) == 'a'
          && getCharAt(position + 4) == 't'
          && getCharAt(position + 5) == 'i'
          && getCharAt(position + 6) == 'l'
          && getCharAt(position + 7) == 'e'
          && keyWordEnd(position + 8)
          ) {
        token_list.emplace_back(TokenType::VOLATILE, line, column);
        position += 8;
        column += 8;
        return true;
      }
    }
    break;
  case 'w':
    if (getCharAt(position + 1) == 'h'
        && getCharAt(position + 2) == 'i'
        && getCharAt(position + 3) == 'l'
        && getCharAt(position + 4) == 'e'
        && keyWordEnd(position + 5)
        ) {
      token_list.emplace_back(TokenType::WHILE, line, column);
      position += 5;
      column += 5;
      return true;
    }
    break;
  case '_':
    switch (getCharAt(position + 1)) {
    case 'A':
      if (getCharAt(position + 2) == 'l'
          && getCharAt(position + 3) == 'i'
          && getCharAt(position + 4) == 'g'
          && getCharAt(position + 5) == 'n'
          ) {
        if (getCharAt(position + 6) == 'a'
            && getCharAt(position + 7) == 's'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(TokenType::ALIGN_AS, line, column);
          position += 8;
          column += 8;
          return true;
        }
        if (getCharAt(position + 6) == 'o'
            && getCharAt(position + 7) == 'f'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(TokenType::ALIGN_OF, line, column);
          position += 8;
          column += 8;
          return true;
        }
      }
      if (getCharAt(position + 2) == 't'
          && getCharAt(position + 3) == 'o'
          && getCharAt(position + 4) == 'm'
          && getCharAt(position + 5) == 'i'
          && getCharAt(position + 6) == 'c'
          && keyWordEnd(position + 7)
          ) {
        token_list.emplace_back(TokenType::ATOMIC, line, column);
        position += 7;
        column += 7;
        return true;
      }
      break;
    case 'B':
      if (getCharAt(position + 2) == 'o'
          && getCharAt(position + 3) == 'o'
          && getCharAt(position + 4) == 'l'
          && keyWordEnd(position + 5)
          ) {
        token_list.emplace_back(TokenType::BOOL, line, column);
        position += 5;
        column += 5;
        return true;
      }
      break;
    case 'C':
      if (getCharAt(position + 2) == 'o'
          && getCharAt(position + 3) == 'm'
          && getCharAt(position + 4) == 'p'
          && getCharAt(position + 5) == 'l'
          && getCharAt(position + 6) == 'e'
          && getCharAt(position + 7) == 'x'
          && keyWordEnd(position + 8)
          ) {
        token_list.emplace_back(TokenType::COMPLEX, line, column);
        position += 8;
        column += 8;
        return true;
      }
      break;
    case 'G':
      if (getCharAt(position + 2) == 'e'
          && getCharAt(position + 3) == 'n'
          && getCharAt(position + 4) == 'e'
          && getCharAt(position + 5) == 'r'
          && getCharAt(position + 6) == 'i'
          && getCharAt(position + 7) == 'c'
          && keyWordEnd(position + 8)
          ) {
        token_list.emplace_back(TokenType::GENERIC, line, column);
        position += 8;
        column += 8;
        return true;
      }
      break;
    case 'I':
      if (getCharAt(position + 2) == 'm'
          && getCharAt(position + 3) == 'a'
          && getCharAt(position + 4) == 'g'
          && getCharAt(position + 5) == 'i'
          && getCharAt(position + 6) == 'n'
          && getCharAt(position + 7) == 'a'
          && getCharAt(position + 8) == 'r'
          && getCharAt(position + 9) == 'y'
          && keyWordEnd(position + 10)
          ) {
        token_list.emplace_back(TokenType::IMAGINARY, line, column);
        position += 10;
        column += 10;
        return true;
      }
      break;
    case 'N':
      if (getCharAt(position + 2) == 'o'
          && getCharAt(position + 3) == 'r'
          && getCharAt(position + 4) == 'e'
          && getCharAt(position + 5) == 't'
          && getCharAt(position + 6) == 'u'
          && getCharAt(position + 7) == 'r'
          && getCharAt(position + 8) == 'n'
          && keyWordEnd(position + 9)
          ) {
        token_list.emplace_back(TokenType::NO_RETURN, line, column);
        position += 9;
        column += 9;
        return true;
      }
      break;
    case 'S':
      if (getCharAt(position + 2) == 't'
          && getCharAt(position + 3) == 'a'
          && getCharAt(position + 4) == 't'
          && getCharAt(position + 5) == 'i'
          && getCharAt(position + 6) == 'c'
          && getCharAt(position + 7) == '_'
          && getCharAt(position + 8) == 'a'
          && getCharAt(position + 9) == 's'
          && getCharAt(position + 10) == 's'
          && getCharAt(position + 11) == 'e'
          && getCharAt(position + 12) == 'r'
          && getCharAt(position + 13) == 't'
          && keyWordEnd(position + 14)
          ) {
        token_list.emplace_back(TokenType::STATIC_ASSERT, line, column);
        position += 14;
        column += 14;
        return true;
      }
      break;
    case 'T':
      if (getCharAt(position + 2) == 'h'
          && getCharAt(position + 3) == 'r'
          && getCharAt(position + 4) == 'e'
          && getCharAt(position + 5) == 'a'
          && getCharAt(position + 6) == 'd'
          && getCharAt(position + 7) == '_'
          && getCharAt(position + 8) == 'l'
          && getCharAt(position + 9) == 'o'
          && getCharAt(position + 10) == 'c'
          && getCharAt(position + 11) == 'a'
          && getCharAt(position + 12) == 'l'
          && keyWordEnd(position + 13)
          ) {
        token_list.emplace_back(TokenType::THREAD_LOCAL, line, column);
        position += 13;
        column += 13;
        return true;
      }
      break;
    default:break;
    }
    break;
  default:break;
  }
  /*
   * Fallthrough, no keyword matched!
   */
  return false;
}

inline bool FastLexer::munch() {
  switch (getCharAt(position)) {
  case 0: return false;
  case ' ':
  case '\t':
  case '\n':
  case '\r':return munchWhitespace();
  case '/':
    if (getCharAt(position + 1) == '/') {
      position += 2;
      return munchLineComment();
    }
    if (getCharAt(position + 1) == '*') {
      position += 2;
      return munchBlockComment();
    }
    //fall-through
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
    if (isPunctuator()) {
      return true;
    }
    return failParsing();
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':return munchNumber();
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
    if (isKeyword()) {
      return true;
    }
    //fall-through
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
  case 'Z':return munchIdentifier();
  case '\'':return munchCharacter();
  case '"':return munchString();
  default:return failParsing();
  }
}

std::vector<Token, std::allocator<Token>> FastLexer::lex() {
  while (munch()) {
  }
  return token_list;
}

} // namespace ccc
