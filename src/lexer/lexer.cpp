#include "lexer.hpp"
#include "reflex_lexer.hpp"

Lexer::Lexer() = default;

std::list<Token, std::allocator<Token>> Lexer::lex(std::string input) {
  ReflexLexer lexer(input);
  lexer.lex();
  return lexer.results();
}
