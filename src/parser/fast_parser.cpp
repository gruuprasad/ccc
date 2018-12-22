#include "fast_parser.hpp"

namespace ccc {

void FastParser::parseExternalDeclaration() {
  std::cout << __FUNCTION__ << std::endl;
  while (curTokenIdx + 1 != tokens.size())
    parseFuncDefOrDeclaration();
}

void FastParser::parseFuncDefOrDeclaration() {
  std::cout << __FUNCTION__ << std::endl;
  parseTypeSpecifiers();
  if (peek().is(TokenType::SEMICOLON) && consume())
    return;
  parseDeclarator();
  switch (peek().getType()) {
  case TokenType::SEMICOLON:
    consume();
    return;
  case TokenType::BRACE_OPEN:
    parseCompoundStatement();
    return;
  default:
    parseDeclarations();
    parseCompoundStatement();
    return;
  }
}

void FastParser::parseCompoundStatement() {
  std::cout << __FUNCTION__ << std::endl;
  expect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      expect(TokenType::SEMICOLON);
    } else {
      parseStatement();
    }
    if (fail())
      return;
  }
  consume();
}

void FastParser::parseBlockItemList() {}

void FastParser::parseStatement() {
  std::cout << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    parseLabeledStatement();
    return;
  case TokenType::BRACE_OPEN:
    parseCompoundStatement();
    return;
  case TokenType::IF:
    parseSelectionStatement();
    return;
  case TokenType::WHILE:
    parseIterationStatement();
    return;
  case TokenType::GOTO:
    consume();
    expect(TokenType::IDENTIFIER);
    break;
  case TokenType::CONTINUE:
    consume();
    break;
  case TokenType::BREAK:
    consume();
    break;
  case TokenType::RETURN:
    consume();
    if (peek().is_not(TokenType::SEMICOLON))
      parseExpression();
    break;
  default:
    if (peek().is_not(TokenType::SEMICOLON))
      parseExpression();
    break;
  }
  expect(TokenType::SEMICOLON);
}

void FastParser::parseLabeledStatement() {
  std::cout << __FUNCTION__ << std::endl;
  expect(TokenType::IDENTIFIER, TokenType::COLON);
  parseStatement();
}

void FastParser::parseSelectionStatement() {
  std::cout << __FUNCTION__ << std::endl;
  expect(TokenType::IF, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  peek().is(TokenType::IF) ? parseSelectionStatement() : parseStatement();
  if (peek().is(TokenType::ELSE) && consume())
    parseStatement();
  return;
}

void FastParser::parseIterationStatement() {
  std::cout << __FUNCTION__ << std::endl;
  expect(TokenType::WHILE, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
}

void FastParser::parseDeclarations() {
  std::cout << __FUNCTION__ << std::endl;
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON);
  } while (peek().is_oneof(C_TYPES));
}

void FastParser::parseTypeSpecifiers() {
  std::cout << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::VOID:
  case TokenType::CHAR:
  case TokenType::SHORT:
  case TokenType::INT:
    nextToken();
    return;
  case TokenType::STRUCT:
    return parseStructOrUnionSpecifier();
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting type specifier");
  }
}

void FastParser::parseDeclarator() {
  std::cout << __FUNCTION__ << std::endl;
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  std::cout << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
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
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting declarator");
  }
}

void FastParser::parseParameterList() {
  std::cout << __FUNCTION__ << std::endl;
  parseList(
      [&]() {
        parseTypeSpecifiers();
        parseDeclarator();
      },
      TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
}

void FastParser::parseStructOrUnionSpecifier() {
  std::cout << __FUNCTION__ << std::endl;
  nextToken();
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    nextToken();
    if (peek().is_not(TokenType::BRACE_OPEN))
      return;
  case TokenType::BRACE_OPEN:
    consume();
    do {
      parseStructDeclaration();
    } while (peek().is_not(TokenType::BRACE_CLOSE));
    expect(TokenType::BRACE_CLOSE);
    return;
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting struct");
  }
}

void FastParser::parseStructDeclaration() {
  std::cout << __FUNCTION__ << std::endl;
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON);
}

// Expressions
void FastParser::parseExpression() {
  std::cout << __FUNCTION__ << std::endl;
  parseUnaryExpression();

  while (true) {
    // Parse binary op and RHS, if there is.
    if (peek().is_oneof(BINARY_OP)) {
      consume();
      parseUnaryExpression();
    } else {
      break;
    }
  }
}

void FastParser::parseUnaryExpression() {
  std::cout << __FUNCTION__ << std::endl;
  if (peek().is_oneof(UNARY_OP) && consume()) {
    if (peek().is(TokenType::PARENTHESIS_OPEN) && consume()) {
      parseTypeSpecifiers();
      expect(TokenType::PARENTHESIS_CLOSE);
      return;
    }
    parseUnaryExpression();
  } else {
    parsePostfixExpression();
  }
}

void FastParser::parsePostfixExpression() {
  std::cout << __FUNCTION__ << std::endl;
  parsePrimaryExpression();
  // optional postfix varients
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN:
    consume();
    parseExpression();
    expect(TokenType::BRACKET_CLOSE);
    return;
  case TokenType::PARENTHESIS_OPEN:
    consume();
    parseArgumentExpressionList();
    expect(TokenType::PARENTHESIS_CLOSE);
    return;
  case TokenType::DOT:
    consume();
    expect(TokenType::IDENTIFIER);
    return;
  case TokenType::ARROW:
    consume();
    expect(TokenType::IDENTIFIER);
    return;
  }
}

void FastParser::parsePrimaryExpression() {
  std::cout << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    nextToken();
    return;
  case TokenType::PARENTHESIS_OPEN:
    consume();
    parseExpression();
    expect(TokenType::PARENTHESIS_CLOSE);
    return;
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", in primary expression.");
  }
}

void FastParser::parseArgumentExpressionList() {
  std::cout << __FUNCTION__ << std::endl;
  // TODO implement
}
void FastParser::parseTranslationUnit() {
  std::cout << __FUNCTION__ << std::endl;
  return parseExternalDeclaration();
}

} // namespace ccc
