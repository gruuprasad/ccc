#ifndef C4_LEXER_HPP
#define C4_LEXER_HPP

#include <string>
#include <list>
#include "token.hpp"
class Lexer {
public:
  Lexer();
  std::list<Token, std::allocator<Token>> lex(std::string input);
};

#endif //C4_LEXER_HPP
