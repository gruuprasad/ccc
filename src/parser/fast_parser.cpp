#include "fast_parser.hpp"
#include "../utils/assert.hpp"

namespace ccc {

void FastParser::parseFuncDefOrDeclaration() {
  parseTypeSpecifiers();
  parseDeclarator();
  if (peek().is(TokenType::SEMICOLON)) {
    consume();
    return;
  }
  error = "Function definition not implemented";
  //parseDeclarations();
  //parse compound-statement
}

void FastParser::parseTypeSpecifiers() {
  while (peek().is_oneof(TokenType::VOID, 
                           TokenType::CHAR,
                           TokenType::SHORT, 
                           TokenType::INT,
                           TokenType::STRUCT)) {
    if (peek().is(TokenType::STRUCT))
      return parseStructOrUnionSpecifier();
    auto token = nextToken();
  }
}

void FastParser::parseDeclarator() {
  // Consume Pointer symbols
  while (peek().is(TokenType::STAR)) {
    consume();
  }
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  switch(peek().getType()) {
    case TokenType::IDENTIFIER:
      nextToken();
      return;
    case TokenType::PARENTHESIS_OPEN:
      consume();
      if (peek().is_oneof(TokenType::VOID, TokenType::CHAR, TokenType::SHORT, 
                          TokenType::INT, TokenType::STRUCT)) {
        parseParameterList();
        expect(TokenType::PARENTHESIS_CLOSE);
        return;
      }
      parseDeclarator();
      expect(TokenType::PARENTHESIS_CLOSE);
      return;
    default:
      error = "Parse Error";
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() << ", expecting Direct Declarator";
  }
}

void FastParser::parseParameterList() {
  do {
    parseTypeSpecifiers();
    parseDeclarator();
  } while (peek().is(TokenType::COMMA) && consume());
}
void FastParser::parseStructOrUnionSpecifier() {
  nextToken();
  switch(peek().getType()) {
    // FIXME RHS -> struct-union identifier clashes with declarator 
    case TokenType::IDENTIFIER:
      nextToken();
      if (peek().is_not(TokenType::BRACE_OPEN)) return;
    case TokenType::BRACE_OPEN:
      consume();
      do {
        parseStructDeclaration();
      } while (peek().is_not(TokenType::BRACE_CLOSE));
      expect(TokenType::BRACE_CLOSE);
      return;
    default:
      error = "Parse Error";
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() << ", expecting struct-or-union";
  }
}

void FastParser::parseStructDeclaration() {
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarators();
  expect(TokenType::SEMICOLON);
}

void FastParser::parseDeclarators() {
  do {
    parseDeclarator();
  } while (peek().is(TokenType::COMMA) && consume());
}

} // namespace ccc
