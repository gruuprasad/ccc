#include "fast_parser.hpp"
#include "../utils/assert.hpp"

namespace ccc {

#define PARSER_ERROR(line, column, msg) std::to_string(line) + ":" + std::to_string(column + 1) + ": error: '" +  msg  +  "'. Parsing Stopped!"

void FastParser::parseExternalDeclaration() {
  while (curTokenIdx != tokens.size())
    parseFuncDefOrDeclaration();
}

void FastParser::parseFuncDefOrDeclaration() {
  parseTypeSpecifiers();
  if (peek().is(TokenType::SEMICOLON) && consume()) return;
  parseDeclarator();
  expect(TokenType::SEMICOLON);
  return;
  my_assert(0) << "TODO: Function definition not implemented";
  //parseDeclarations();
  //parse compound-statement
}

void FastParser::parseTypeSpecifiers() {
  switch(peek().getType()) {
    case TokenType::VOID:
    case TokenType::CHAR:
    case TokenType::SHORT:
    case TokenType::INT:
      nextToken(); return;
    case TokenType::STRUCT:
      return parseStructOrUnionSpecifier();
    default:
      error = "Parse Error";
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() 
                   << ", expecting type specifier";
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
      if (peek().is(TokenType::PARENTHESIS_OPEN) && consume())
        parseParameterList();
      return;
    case TokenType::PARENTHESIS_OPEN:
      consume();
      parseDeclarator();
      expect(TokenType::PARENTHESIS_CLOSE);
      if (peek().is(TokenType::PARENTHESIS_OPEN) && consume())
        parseParameterList();
      return;
    default:
      error = "Parse Error";
      my_assert(0) << "Parse Error: Unexpected Token: " << peek().name() 
                   << ", expecting Direct Declarator";
  }
}

void FastParser::parseParameterList() {
  parseList([&] () { parseTypeSpecifiers(); parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
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
