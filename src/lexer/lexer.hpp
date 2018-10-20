#ifndef C4_LEXER_HPP
#define C4_LEXER_HPP

#include <string>
class Lexer {
public:
  Lexer();
  int lex(std::string input);
};

#endif //C4_LEXER_HPP
