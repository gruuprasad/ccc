#include "fast_parser.hpp"
#include "../utils/assert.hpp"

namespace ccc {

void FastParser::parseExternalDeclaration() {
  while (curTokenIdx != tokens.size())
    parseFuncDefOrDeclaration();
}

void FastParser::parseFuncDefOrDeclaration() {
  parseTypeSpecifiers();
  switch(peek().getType()) {
    case TokenType::SEMICOLON:
      consume(); return;
    default:
    parseDeclarator();
    expect(TokenType::SEMICOLON);
    return;
  }
  my_assert(0) << "TODO: Function definition not implemented";
  //parseDeclarations();
  //parse compound-statement
}

void FastParser::parseTypeSpecifiers() {
  while (peek().is_oneof(C_TYPES)) {
    if (peek().is(TokenType::STRUCT))
      return parseStructOrUnionSpecifier();
    auto token = nextToken();
  }
}

void FastParser::parseDeclarator() {
  parseList([&] () {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  switch(peek().getType()) {
    case TokenType::IDENTIFIER:
      nextToken();
      return;
    case TokenType::PARENTHESIS_OPEN:
      consume();
      if (peek().is_oneof(C_TYPES)) {
        parseList([&] () { parseTypeSpecifiers(); parseDeclarator(); }, TokenType::COMMA);
        expect(TokenType::PARENTHESIS_CLOSE);
        return;
      }
      parseDeclarator();
      expect(TokenType::PARENTHESIS_CLOSE);
      return;
    default:
      error = "Parse Error";
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() 
                   << ", expecting Direct Declarator";
  }
}

void FastParser::parseStructOrUnionSpecifier() {
  nextToken();
  switch(peek().getType()) {
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
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() 
                   << ", expecting struct-or-union";
  }
}

void FastParser::parseStructDeclaration() {
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&] () { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON);
}

} // namespace ccc
