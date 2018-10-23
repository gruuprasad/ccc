#include <sstream>
#include "fast_lexer.hpp"
FastLexer::FastLexer(const std::string &content) : content(content) {
  token_list = std::list<Token, std::allocator<Token>>();
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

inline bool FastLexer::isKeyword() {
  const char first = getCharAt(position);
  switch (first) {
  case 'a':
    if (getCharAt(position + 1) == 'u'
        && getCharAt(position + 2) == 't'
        && getCharAt(position + 3) == 'o'
        && keyWordEnd(position + 4)
        ) {
      token_list.emplace_back(Token(TokenType::AUTO, line, column, ""));
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
      token_list.emplace_back(Token(TokenType::BREAK, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::CASE, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::CHAR, line, column, ""));
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
          token_list.emplace_back(Token(TokenType::CONST, line, column, ""));
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
          token_list.emplace_back(Token(TokenType::CONTINUE, line, column, ""));
          position += 8;
          column += 8;
          return true;
        }
      }
      break;
    default:break;
    }
  case 'd':
    if (getCharAt(position + 1) == 'o'
        && keyWordEnd(position + 2)
        ) {
      token_list.emplace_back(Token(TokenType::DO, line, column, ""));
      position += 2;
      column += 2;
      return true;
    }
    if (getCharAt(position + 1) == 'e'
        && getCharAt(position + 2) == 'f'
        && getCharAt(position + 3) == 'a'
        && getCharAt(position + 4) == 'u'
        && getCharAt(position + 5) == 'l'
        && getCharAt(position + 6) == 't'
        && keyWordEnd(position + 7)
        ) {
      token_list.emplace_back(Token(TokenType::DEFAULT, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::ELSE, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::ENUM, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::EXTERN, line, column, ""));
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
      token_list.emplace_back(Token(TokenType::FOR, line, column, ""));
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
      token_list.emplace_back(Token(TokenType::GOTO, line, column, ""));
      position += 4;
      column += 4;
      return true;
    }
    break;
  case 'i':
    switch (getCharAt(position + 1)) {
    case 'f':
      if (keyWordEnd(position + 2)) {
        token_list.emplace_back(Token(TokenType::IF, line, column, ""));
        position += 2;
        column += 2;
        return true;
      }
      break;
    case 'n':
      if (getCharAt(position + 2) == 't'
          && keyWordEnd(position + 3)
          ) {
        token_list.emplace_back(Token(TokenType::INT, line, column, ""));
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
        token_list.emplace_back(Token(TokenType::INLINE, line, column, ""));
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
      token_list.emplace_back(Token(TokenType::LONG, line, column, ""));
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
          token_list.emplace_back(Token(TokenType::REGISTER, line, column, ""));
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
          token_list.emplace_back(Token(TokenType::RESTRICT, line, column, ""));
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
          token_list.emplace_back(Token(TokenType::RETURN, line, column, ""));
          position += 6;
          column += 6;
          return true;
        }
        break;
      default:break;
      }
    }
    break;
  default:break;
  }
  /*
   * Fallthrough, no keyword matched!
   */
  return false;
}

bool FastLexer::munch() {
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
    case '\n':column = 0;
      ++line;
      break;
    default:++column;
      break;
    }
    first = getCharAt(++position);
  }
  unsigned long oldPosition = position;
  std::stringstream tokenStream;

  /*
   * Check if we have a number at hand
   */
  if ('0' <= first && first <= '9') {
    do {
      tokenStream << first;
      first = getCharAt(++position);
    } while ('0' <= first && first <= '9');
    token_list.emplace_back(Token(TokenType::NUMBER, line, column, tokenStream.str()));
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
      tokenStream << first;
      first = getCharAt(++position);
    } while (('0' <= first && first <= '9')
        || ('a' <= first && first <= 'z')
        || ('A' <= first && first <= 'Z')
        || first == '_');
    token_list.emplace_back(Token(TokenType::IDENTIFIER, line, column, tokenStream.str()));
    column += position - oldPosition;
    return true;
  }

  /*
   * Check if we have a character constant
   */
  if (first == '\'') {

  }

  /*
   * Check if we have a string literal
   */
  if (first == '"') {

  }

  /*
   * We matched nothing, we should fail the lexing!
   */
  return false;
}

std::list<Token, std::allocator<Token>> FastLexer::lex() {
  while (munch()) {
  }
  return token_list;
}


