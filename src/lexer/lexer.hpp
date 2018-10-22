#ifndef C4_LEXER_HPP
#define C4_LEXER_HPP

#include <string>
#include <list>
#include "token.hpp"
#include "../reflex/input.h"
class Lexer {
public:
  Lexer();
  std::list<Token, std::allocator<Token>> lex(const reflex::Input& input);
};

#endif //C4_LEXER_HPP
