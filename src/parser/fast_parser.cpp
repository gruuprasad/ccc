#include "fast_parser.hpp"

namespace ccc {

void FastParser::parseExternalDeclaration() {
  while (curTokenIdx + 1 != tokens.size())
    parseFuncDefOrDeclaration();
}

void FastParser::parseFuncDefOrDeclaration() {
  parseTypeSpecifiers();
  if (peek().is(TokenType::SEMICOLON) && consume()) return;
  parseDeclarator();
  switch(peek().getType()) {
    case TokenType::SEMICOLON: 
      consume(); return;
    case TokenType::BRACE_OPEN:
      parseCompoundStatement(); return;
    default:
      parseDeclarations();
      parseCompoundStatement();
      return;
  }
}

void FastParser::parseCompoundStatement() {
  expect(TokenType::BRACE_OPEN);
  do {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      expect(TokenType::SEMICOLON);
    } else {
      parseStatement();
    }
  } while(peek().is_not(TokenType::BRACE_CLOSE));
  consume();
}

void FastParser::parseBlockItemList() {
}

void FastParser::parseStatement() {
  switch (peek().getType()) {
    case TokenType::IDENTIFIER: parseLabeledStatement(); return;
    case TokenType::BRACE_OPEN: parseCompoundStatement(); return;
    case TokenType::IF: parseSelectionStatement(); return;
    case TokenType::WHILE: parseIterationStatement(); return;
    case TokenType::GOTO: consume(); expect(TokenType::IDENTIFIER); break;
    case TokenType::CONTINUE: consume(); break;
    case TokenType::BREAK: consume(); break;
    case TokenType::RETURN: consume(); 
      if (peek().is_not(TokenType::SEMICOLON)) 
        parseExpression(); 
      break;
    default: 
      if(peek().is_not(TokenType::SEMICOLON)) 
          parseExpression(); 
      break;
  }
  expect(TokenType::SEMICOLON);
}

void FastParser::parseLabeledStatement() {
  expect(TokenType::IDENTIFIER, TokenType::COLON);
  parseStatement();
}

void FastParser::parseSelectionStatement() {
  expect(TokenType::IF, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  peek().is(TokenType::IF) ? parseSelectionStatement() : parseStatement();
  if (peek().is(TokenType::ELSE) && consume()) parseStatement();
  return;
}

void FastParser::parseIterationStatement() {
  expect(TokenType::WHILE, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
}

void FastParser::parseDeclarations() {
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON);
  } while (peek().is_oneof(C_TYPES));
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
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(), 
                          "Unexpected Token: \"" + peek().name() + "\", expecting type specifier");
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
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(), 
                          "Unexpected Token: \"" + peek().name() + "\", expecting declarator");
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
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(), 
                          "Unexpected Token: \"" + peek().name() + "\", expecting struct");
  }
}

void FastParser::parseStructDeclaration() {
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&] () { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON);
}

// Expressions
void FastParser::parsePrimaryExpression() {
  switch(peek().getType()) {
    case TokenType::IDENTIFIER:
    case TokenType::NUMBER:
    case TokenType::CHARACTER:
    case TokenType::STRING:
      nextToken(); return;
    case TokenType::PARENTHESIS_OPEN:
      consume();
      parseExpression();
      expect(TokenType::PARENTHESIS_CLOSE);
      return;
    default:
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(), 
                          "Unexpected Token: \"" + peek().name() + "\", in primary expression.");
  }
}

/*
void Token::parsePostfix() {

}*/

void FastParser::parseExpression() {
  parsePrimaryExpression();
}

} // namespace ccc
