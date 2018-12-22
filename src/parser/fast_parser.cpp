#include "fast_parser.hpp"

namespace ccc {

ASTNode *FastParser::parseExternalDeclaration() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  std::vector<ASTNode *> block;
  const Token *start = &peek();
  while (!peek().is(TokenType::_EOF))
    block.push_back(parseFuncDefOrDeclaration());
  return new Ghost(count++, start, block);
}

ASTNode *FastParser::parseFuncDefOrDeclaration() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }

  if (peek().is_oneof(C_TYPES)) {
    parseTypeSpecifiers();
    if (peek().is(TokenType::SEMICOLON) && consume(__FUNCTION__))
      return nullptr;
    else {
      parseDeclarator();
      expect(TokenType::SEMICOLON, __FUNCTION__);
    }
  } else {
    switch (peek().getType()) {
    case TokenType::SEMICOLON:
      consume(__FUNCTION__);
      return nullptr;
    case TokenType::BRACE_OPEN:
      return parseCompoundStatement();
    }
  }
  return nullptr;
} // namespace ccc

void FastParser::print_token() {
  //  for (int i = curTokenIdx; i < tokens.size(); i++) {
  //    std::cout << tokens[i].name();
  //  }
  //  std::cout << std::endl;
}

Statement *FastParser::parseCompoundStatement() {
  std::vector<ASTNode *> block;
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  const Token *brace = &peek();
  expect(TokenType::BRACE_OPEN, __FUNCTION__);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      block.push_back(new Ghost(count++, &peek()));
      expect(TokenType::SEMICOLON, __FUNCTION__);
    } else {
      block.push_back(parseStatement());
    }
  }
  expect(TokenType::BRACE_CLOSE, __FUNCTION__);
  return new CompoundStatement(count++, brace, block);
}

void FastParser::parseBlockItemList() {}

Statement *FastParser::parseStatement() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::SEMICOLON: {
    expect(TokenType::SEMICOLON, __FUNCTION__);
    return nullptr;
  }
  case TokenType::BRACE_OPEN:
    return parseCompoundStatement();
  case TokenType::IF:
    parseSelectionStatement();
    return nullptr;
  case TokenType::WHILE:
    parseIterationStatement();
    return nullptr;
  case TokenType::GOTO:
    consume(__FUNCTION__);
    expect(TokenType::IDENTIFIER);
    break;
  case TokenType::CONTINUE:
    consume(__FUNCTION__);
    break;
  case TokenType::BREAK:
    consume(__FUNCTION__);
    break;
  case TokenType::RETURN:
    consume(__FUNCTION__);
    if (peek().is_not(TokenType::SEMICOLON))
      parseExpression();
    break;
  case TokenType::IDENTIFIER: {
    if (peek(1).is(TokenType::COLON)) {
      parseLabeledStatement();
      return nullptr;
    }
  }
  default:
    const Token *start = &peek();
    Expression *exp = parseExpression();
    expect(TokenType::SEMICOLON, __FUNCTION__);
    return new ExpressionStatement(count++, start, exp);
  }
  expect(TokenType::SEMICOLON, __FUNCTION__);
}

void FastParser::parseLabeledStatement() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  expect(TokenType::IDENTIFIER);
  expect(TokenType::COLON);
  parseStatement();
}

void FastParser::parseSelectionStatement() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  expect(TokenType::IF);
  expect(TokenType::PARENTHESIS_OPEN);

  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  if (peek().is(TokenType::IF))
    parseSelectionStatement();
  else
    parseStatement();
  if (peek().is(TokenType::ELSE) && consume(__FUNCTION__))
    parseStatement();
  return;
}

void FastParser::parseIterationStatement() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  expect(TokenType::WHILE);
  expect(TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
}

void FastParser::parseDeclarations() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON, __FUNCTION__);
  } while (peek().is_oneof(C_TYPES));
}

void FastParser::parseTypeSpecifiers() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::VOID:
  case TokenType::CHAR:
  case TokenType::SHORT:
  case TokenType::INT:
    consume(__FUNCTION__);
    return;
  case TokenType::STRUCT:
    consume(__FUNCTION__);
    return parseStructOrUnionSpecifier();
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting type specifier");
  }
}

void FastParser::parseDeclarator() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      consume(__FUNCTION__);
      parseParameterList();
    } else {
      parseExpression();
      return;
    }
  case TokenType::PARENTHESIS_OPEN:
    consume(__FUNCTION__);
    parseDeclarator();
    expect(TokenType::PARENTHESIS_CLOSE);
    if (peek().is(TokenType::PARENTHESIS_OPEN) && consume(__FUNCTION__))
      parseParameterList();
    return;
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting declarator");
  }
} // namespace ccc

void FastParser::parseParameterList() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  parseList(
      [&]() {
        parseTypeSpecifiers();
        parseDeclarator();
      },
      TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
}

void FastParser::parseStructOrUnionSpecifier() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    consume(__FUNCTION__);
    if (peek().is_not(TokenType::BRACE_OPEN))
      return;
  case TokenType::BRACE_OPEN:
    consume(__FUNCTION__);
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
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON, __FUNCTION__);
}

// Expressions
Expression *FastParser::parseExpression() { // TODO fix order of ops s. lecture
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  auto exp = parseUnaryExpression();
  if (peek().is_oneof(BINARY_OP)) {
    if (this->debug) {
      std::cout << "===> " << peek() << " -> "
                << "parseBinaryExpression" << std::endl;
    }
    consume(__FUNCTION__);
    switch (peek(-1).getType()) {
    case TokenType::STAR:
      return new MultiplicativeExpression(count++, exp, &peek(-1),
                                          parseExpression());
    case TokenType::PLUS:
    case TokenType::MINUS:
      return new AdditiveExpression(count++, exp, &peek(-1), parseExpression());
    case TokenType::LESS:
      return new RelationalExpression(count++, exp, &peek(-1),
                                      parseExpression());
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
      return new EqualityExpression(count++, exp, &peek(-1), parseExpression());
    case TokenType::AND:
      return new LogicalAndExpression(count++, exp, &peek(-1),
                                      parseExpression());
    case TokenType::OR:
      return new LogicalOrExpression(count++, exp, &peek(-1),
                                     parseExpression());
    case TokenType::ASSIGN:
      return new AssignmentExpression(count++, exp, &peek(-1),
                                      parseExpression());
    case TokenType::PLUS_ASSIGN:
      return new AssignmentExpression(count++, exp, &peek(-1),
                                      parseExpression());
    }
  } else
    return exp;
}

Expression *FastParser::parseUnaryExpression() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  if (peek().is_oneof(UNARY_OP) && consume(__FUNCTION__)) {
    if (peek().is(TokenType::PARENTHESIS_OPEN) && consume(__FUNCTION__)) {
      parseTypeSpecifiers();
      expect(TokenType::PARENTHESIS_CLOSE);
      return nullptr;
    }
    return parseUnaryExpression();
  } else {
    return parsePostfixExpression();
  }
}

Expression *FastParser::parsePostfixExpression() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  Expression *exp = parsePrimaryExpression();
  // optional postfix varients
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN: {
    consume(__FUNCTION__);
    Expression *exp = parseExpression();
    expect(TokenType::BRACKET_CLOSE);
    return exp;
  }
  case TokenType::PARENTHESIS_OPEN:
    consume(__FUNCTION__);
    parseArgumentExpressionList();
    expect(TokenType::PARENTHESIS_CLOSE);
    return nullptr;
  case TokenType::DOT:
    consume(__FUNCTION__);
    expect(TokenType::IDENTIFIER);
    return nullptr;
  case TokenType::ARROW:
    consume(__FUNCTION__);
    expect(TokenType::IDENTIFIER);
    return nullptr;
  }
  return exp;
}

Expression *FastParser::parsePrimaryExpression() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    consume(__FUNCTION__);
    return new PrimaryExpression(count++, &peek(-1));
  case TokenType::PARENTHESIS_OPEN: {
    consume(__FUNCTION__);
    Expression *exp = parseExpression();
    expect(TokenType::PARENTHESIS_CLOSE);
    return exp;
  }
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", in primary expression.");
  }
  return nullptr;
}

void FastParser::parseArgumentExpressionList() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  // TODO implement
}
ASTNode *FastParser::parseTranslationUnit() {
  if (this->debug) {
    print_token();
    std::cout << "===> " << peek() << " -> " << __FUNCTION__ << std::endl;
  }
  return new Ghost(count++, &peek(), parseExternalDeclaration());
}

} // namespace ccc
