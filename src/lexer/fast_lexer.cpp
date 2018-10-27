#include <iostream>
#include "lexer_exception.hpp"
#include "fast_lexer.hpp"

FastLexer::FastLexer(const std::string &content) : content(content) {
  token_list = std::vector<Token, std::allocator<Token>>();
  token_list.reserve(content.size() >> 1);
  position = 0;
  line = 1;
  column = 0;
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

inline bool FastLexer::isPunctuator() {
  const char first = getCharAt(position);
  switch (first) {
  case '{':token_list.emplace_back(Token(TokenType::BRACE_OPEN, line, column));
    ++position;
    ++column;
    return true;
  case '}':token_list.emplace_back(Token(TokenType::BRACE_CLOSE, line, column));
    ++position;
    ++column;
    return true;
  case '[':token_list.emplace_back(Token(TokenType::BRACKET_OPEN, line, column));
    ++position;
    ++column;
    return true;
  case ']':token_list.emplace_back(Token(TokenType::BRACKET_CLOSE, line, column));
    ++position;
    ++column;
    return true;
  case '(':token_list.emplace_back(Token(TokenType::PARENTHESIS_OPEN, line, column));
    ++position;
    ++column;
    return true;
  case ')':token_list.emplace_back(Token(TokenType::PARENTHESIS_CLOSE, line, column));
    ++position;
    ++column;
    return true;
  case '+':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::PLUS_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '+') {
      token_list.emplace_back(Token(TokenType::PLUSPLUS, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::PLUS, line, column));
    ++position;
    ++column;
    return true;
  case '-':
    switch (getCharAt(position + 1)) {
    case '-':token_list.emplace_back(Token(TokenType::MINUSMINUS, line, column));
      position += 2;
      column += 2;
      return true;
    case '=':token_list.emplace_back(Token(TokenType::MINUS_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    case '>':
      if (getCharAt(position + 2) == '*') {
        token_list.emplace_back(Token(TokenType::ARROW_STAR, line, column));
        position += 3;
        column += 3;
        return true;
      }
      token_list.emplace_back(Token(TokenType::ARROW, line, column));
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(Token(TokenType::MINUS, line, column));
      ++position;
      ++column;
      return true;
    }
    break;
  case '=':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::EQUAL, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::ASSIGN, line, column));
    ++position;
    ++column;
    return true;
  case '<':
    switch (getCharAt(position + 1)) {
    case ':':token_list.emplace_back(Token(TokenType::BRACKET_OPEN_ALT, line, column));
      position += 2;
      column += 2;
      return true;
    case '%':token_list.emplace_back(Token(TokenType::BRACE_OPEN_ALT, line, column));
      position += 2;
      column += 2;
      return true;
    case '=':token_list.emplace_back(Token(TokenType::LESS_EQUAL, line, column));
      position += 2;
      column += 2;
      return true;
    case '<':
      if (getCharAt(position + 2) == '=') {
        token_list.emplace_back(Token(TokenType::LEFT_SHIFT_ASSIGN, line, column));
        position += 3;
        column += 3;
        return true;
      }
      token_list.emplace_back(Token(TokenType::LEFT_SHIFT, line, column));
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(Token(TokenType::LEFT, line, column));
      ++position;
      ++column;
      return true;
    }
    break;
  case '>':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::GREATER_EQUAL, line, column));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '>') {
      if (getCharAt(position + 2) == '=') {
        token_list.emplace_back(Token(TokenType::RIGHT_SHIFT_ASSIGN, line, column));
        position += 3;
        column += 3;
        return true;
      }
      token_list.emplace_back(Token(TokenType::RIGHT_SHIFT, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::RIGHT, line, column));
    ++position;
    ++column;
    return true;
  case '!':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::NOT_EQUAL, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::NOT, line, column));
    ++position;
    ++column;
    return true;
  case ',':token_list.emplace_back(Token(TokenType::COMMA, line, column));
    ++position;
    ++column;
    return true;
  case ';':token_list.emplace_back(Token(TokenType::SEMICOLON, line, column));
    ++position;
    ++column;
    return true;
  case '.':
    if (getCharAt(position + 1) == '.'
        && getCharAt(position + 2) == '.') {
      token_list.emplace_back(Token(TokenType::TRI_DOTS, line, column));
      position += 3;
      column += 3;
      return true;
    }
    if (getCharAt(position + 1) == '*') {
      token_list.emplace_back(Token(TokenType::DOT_STAR, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::DOT, line, column));
    ++position;
    ++column;
    return true;
  case '^':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::CARET_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::CARET, line, column));
    ++position;
    ++column;
    return true;
  case '~':token_list.emplace_back(Token(TokenType::TILDE, line, column));
    ++position;
    ++column;
    return true;
  case '*':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::STAR_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::STAR, line, column));
    ++position;
    ++column;
    return true;
  case '/':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::DIV_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::DIV, line, column));
    ++position;
    ++column;
    return true;
  case '%':
    switch (getCharAt(position + 1)) {
    case '=':token_list.emplace_back(Token(TokenType::MOD_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    case ':':
      if(getCharAt(position + 2) == '%'
      && getCharAt(position + 3) == ':'){
        token_list.emplace_back(Token(TokenType::HASHHASH_ALT, line, column));
        position += 4;
        column += 4;
        return true;
      }
      token_list.emplace_back(Token(TokenType::HASH_ALT, line, column));
      position += 2;
      column += 2;
      return true;
    case '>':token_list.emplace_back(Token(TokenType::BRACE_CLOSE_ALT, line, column));
      position += 2;
      column += 2;
      return true;
    default:token_list.emplace_back(Token(TokenType::MOD, line, column));
      ++position;
      ++column;
      return true;
    }
  case '&':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::AMPERSAND_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '&') {
      token_list.emplace_back(Token(TokenType::AND, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::AMPERSAND, line, column));
    ++position;
    ++column;
    return true;
  case '|':
    if (getCharAt(position + 1) == '=') {
      token_list.emplace_back(Token(TokenType::PIPE_ASSIGN, line, column));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '|') {
      token_list.emplace_back(Token(TokenType::OR, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::PIPE, line, column));
    ++position;
    ++column;
    return true;
  case ':':
    if (getCharAt(position + 1) == ':') {
      token_list.emplace_back(Token(TokenType::COLON_COLON, line, column));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == '>') {
      token_list.emplace_back(Token(TokenType::BRACKET_CLOSE_ALT, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::COLON, line, column));
    ++position;
    ++column;
    return true;
  case '#':
    if (getCharAt(position + 1) == '#') {
      token_list.emplace_back(Token(TokenType::HASHHASH, line, column));
      position += 2;
      column += 2;
      return true;
    }
    token_list.emplace_back(Token(TokenType::HASH, line, column));
    ++position;
    ++column;
    return true;
  case '?':token_list.emplace_back(Token(TokenType::QUESTION, line, column));
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
      token_list.emplace_back(Token(TokenType::AUTO, line, column));
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
      token_list.emplace_back(Token(TokenType::BREAK, line, column));
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
        token_list.emplace_back(Token(TokenType::CASE, line, column));
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
        token_list.emplace_back(Token(TokenType::CHAR, line, column));
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
          token_list.emplace_back(Token(TokenType::CONST, line, column));
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
          token_list.emplace_back(Token(TokenType::CONTINUE, line, column));
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
    if (getCharAt(position + 1) == 'o'){
      if (getCharAt(position + 2) == 'u'
          && getCharAt(position + 3) == 'b'
          && getCharAt(position + 4) == 'l'
          && getCharAt(position + 5) == 'e'
          && keyWordEnd(position + 6)
          ) {
        token_list.emplace_back(Token(TokenType::DOUBLE, line, column));
        position += 6;
        column += 6;
        return true;
      }
        if(keyWordEnd(position + 2)) {
          token_list.emplace_back(Token(TokenType::DO, line, column));
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
      token_list.emplace_back(Token(TokenType::DEFAULT, line, column));
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
        token_list.emplace_back(Token(TokenType::ELSE, line, column));
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
        token_list.emplace_back(Token(TokenType::ENUM, line, column));
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
        token_list.emplace_back(Token(TokenType::EXTERN, line, column));
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
      token_list.emplace_back(Token(TokenType::FOR, line, column));
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
      token_list.emplace_back(Token(TokenType::FLOAT, line, column));
      position += 3;
      column += 3;
      return true;
    }
    break;
  case 'g':
    if (getCharAt(position + 1) == 'o'
        && getCharAt(position + 2) == 't'
        && getCharAt(position + 3) == 'o'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(Token(TokenType::GOTO, line, column));
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'i':
    switch (getCharAt(position + 1)) {
    case 'f':
      if (keyWordEnd(position + 2)) {
        token_list.emplace_back(Token(TokenType::IF, line, column));
        position += 2;
        column += 2;
        return true;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 't'
          && keyWordEnd(position + 3)
          ) {
        token_list.emplace_back(Token(TokenType::INT, line, column));
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
        token_list.emplace_back(Token(TokenType::INLINE, line, column));
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
      token_list.emplace_back(Token(TokenType::LONG, line, column));
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
          token_list.emplace_back(Token(TokenType::REGISTER, line, column));
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
          token_list.emplace_back(Token(TokenType::RESTRICT, line, column));
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
          token_list.emplace_back(Token(TokenType::RETURN, line, column));
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
        token_list.emplace_back(Token(TokenType::SHORT, line, column));
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
        token_list.emplace_back(Token(TokenType::SIGNED, line, column));
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
        token_list.emplace_back(Token(TokenType::SIZEOF, line, column));
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
        token_list.emplace_back(Token(TokenType::STATIC, line, column));
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
        token_list.emplace_back(Token(TokenType::STRUCT, line, column));
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
        token_list.emplace_back(Token(TokenType::SWITCH, line, column));
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
      token_list.emplace_back(Token(TokenType::TYPEDEF, line, column));
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
        token_list.emplace_back(Token(TokenType::UNION, line, column));
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
        token_list.emplace_back(Token(TokenType::UNSIGNED, line, column));
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
        token_list.emplace_back(Token(TokenType::VOID, line, column));
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
        token_list.emplace_back(Token(TokenType::VOLATILE, line, column));
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
      token_list.emplace_back(Token(TokenType::WHILE, line, column));
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
          token_list.emplace_back(Token(TokenType::ALIGN_AS, line, column));
          position += 8;
          column += 8;
          return true;
        }
        if (getCharAt(position + 6) == 'o'
            && getCharAt(position + 7) == 'f'
            && keyWordEnd(position + 8)
            ) {
          token_list.emplace_back(Token(TokenType::ALIGN_OF, line, column));
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
        token_list.emplace_back(Token(TokenType::ATOMIC, line, column));
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
        token_list.emplace_back(Token(TokenType::BOOL, line, column));
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
        token_list.emplace_back(Token(TokenType::COMPLEX, line, column));
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
        token_list.emplace_back(Token(TokenType::GENERIC, line, column));
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
        token_list.emplace_back(Token(TokenType::IMAGINARY, line, column));
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
        token_list.emplace_back(Token(TokenType::NO_RETURN, line, column));
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
        token_list.emplace_back(Token(TokenType::STATIC_ASSERT, line, column));
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
        token_list.emplace_back(Token(TokenType::THREAD_LOCAL, line, column));
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
  char first = getCharAt(position);

  /*
   * At end of file, return
   */
  if (first == 0) {
    return false;
  }

  /*
   * Munch away all whitespace
   */
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
  unsigned long oldPosition = position;

  if (first == 0) {
    return false;
  }

  /*
   * Munch away line comments
   */
  if (first == '/' && getCharAt(position + 1) == '/') {
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

  /*
   * Munch away block comments
   */
  if (first == '/' && getCharAt(position + 1) == '*') {
    unsigned long previousLine = line;
    unsigned long previousColumn = column;
    position += 2;
    column += 2;
    first = getCharAt(position);
    while (first != '*' || getCharAt(position + 1) != '/') {
      switch (first) {
      case 0:throw LexerException(Token(TokenType::QUESTION, previousLine, previousColumn, "Unterminated Comment!"));
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

  /*
   * Check if we have a number at hand
   */
  if ('0' <= first && first <= '9') {
    do {
      first = getCharAt(++position);
    } while ('0' <= first && first <= '9');
    token_list.emplace_back(Token(TokenType::NUMBER, line, column, std::string(&content[oldPosition], position - oldPosition)));
    column += position - oldPosition;
    return true;
  }

  /*
   * Check if we have an identifier or a keyword at hand
   */
  if (('a' <= first && first <= 'z')
      || ('A' <= first && first <= 'Z')
      || first == '_') {
    if (isKeyword()) {
      /*
       * We found a keyword already munched, return
       */
      return true;
    }

    /*
     * No keyword, check for identifier
     */
    do {
      first = getCharAt(++position);
    } while (('0' <= first && first <= '9')
        || ('a' <= first && first <= 'z')
        || ('A' <= first && first <= 'Z')
        || first == '_');
    token_list.emplace_back(Token(TokenType::IDENTIFIER, line, column, std::string(&content[oldPosition], position - oldPosition)));
    column += position - oldPosition;
    return true;
  }

  /*
   * Check if we have a character constant
   */
  if (first == '\'') {
    first = getCharAt(++position);
    if (first != '\''
        && first != '\\'
        && first != '\n'
        && first != '\r'
        && getCharAt(position + 1) == '\'') {
      token_list.emplace_back(Token(TokenType::CHARACTER, line, column, std::string(1, first)));
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
      case 'v':token_list.emplace_back(Token(TokenType::CHARACTER, line, column, std::string(&content[position - 1], 2)));
        column += 4;
        position += 2;
        return true;
      default:
        throw LexerException(Token{TokenType::CHARACTER, line, column,
                                   "Invalid character: '" + std::string(&content[position - 1], 2) + "'"});
      }
    }
    throw LexerException(Token{TokenType::CHARACTER, line, column,
                               "Invalid character: '" + std::string(1, first) + "'"});
  }

  /*
   * Check if we have a string literal
   */
  if (first == '"') {
    unsigned long initColumn = column;
    first = getCharAt(++position);
    ++column;
    while (first != '"') {
      if (first == '\n'
          || first == '\r'
          || first == 0) {
        throw LexerException(Token(TokenType::STRING, line, column,
                                   "Line break in string at " + std::string(&content[oldPosition + 1], position - oldPosition)));
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
          throw LexerException(Token(TokenType::STRING, line, column,
                                     "Invalid escape at " + std::string(&content[oldPosition + 1], position - oldPosition)));
        }
      }
      first = getCharAt(++position);
      ++column;
    }
    token_list.emplace_back(Token(TokenType::STRING, line, initColumn, std::string(&content[oldPosition + 1], position - oldPosition - 1)));
    ++position;
    ++column;
    return true;
  }

  if (isPunctuator()) {
    /*
     * We found a punctuator already munched, return
     */
    return true;
  }

  /*
   * We matched nothing, we should fail the lexing!
   */
  std::string error = "Unknown token " + std::string(1, first);
  if (getCharAt(position + 1) != 0) {
    error += std::string(1, getCharAt(position + 1));
    if (getCharAt(position + 2) != 0) {
      error += std::string(1, getCharAt(position + 2));
      if (getCharAt(position + 3) != 0) {
        error += std::string(1, getCharAt(position + 2)) + " [truncated]";
      }
    }
  }
  throw LexerException(Token(TokenType::QUESTION, line, column, error));
}

std::vector<Token, std::allocator<Token>> FastLexer::lex() {
  while (munch()) {
  }
  return token_list;
}
