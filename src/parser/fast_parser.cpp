#include "fast_parser.hpp"

namespace ccc {

ASTNode *FastParser::parseExternalDeclaration() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  std::vector<ASTNode *> block;
  const Token *start = &peek();
  while (!peek().is(TokenType::ENDOFFILE))
    block.push_back(parseFuncDefOrDeclaration());
  return new Ghost(idx++, start, block);
}

ASTNode *FastParser::parseFuncDefOrDeclaration() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    default:
      break;
    }
  }
  return nullptr;
}

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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  const Token *brace = &peek();
  expect(TokenType::BRACE_OPEN, __FUNCTION__);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      block.push_back(new Ghost(idx++, &peek()));
      expect(TokenType::SEMICOLON, __FUNCTION__);
    } else {
      block.push_back(parseStatement());
    }
  }
  expect(TokenType::BRACE_CLOSE, __FUNCTION__);
  return new CompoundStatement(idx++, brace, block);
}

void FastParser::parseBlockItemList() {}

Statement *FastParser::parseStatement() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::SEMICOLON: {
    expect(TokenType::SEMICOLON, __FUNCTION__);
    return nullptr;
  }
  case TokenType::BRACE_OPEN:
    return parseCompoundStatement();
  case TokenType::IF:
    return parseSelectionStatement();
  case TokenType::WHILE:
    return parseIterationStatement();
  case TokenType::GOTO:
    consume(__FUNCTION__);
    expect(TokenType::IDENTIFIER);
    break;
  case TokenType::CONTINUE:
    return new ContinueStatement(idx++, pop(__FUNCTION__));
  case TokenType::BREAK:
    return new BreakStatement(idx++, pop(__FUNCTION__));
  case TokenType::RETURN: {
    const Token *token = &peek();
    consume(__FUNCTION__);
    if (peek().is_not(TokenType::SEMICOLON)) {
      Expression *exp = parseExpression();
      expect(TokenType::SEMICOLON, __FUNCTION__);
      return new ReturnStatement(idx++, token, exp);
    }
    consume(__FUNCTION__);
    return new ReturnStatement(idx++, token);
  }
  case TokenType::IDENTIFIER: {
    if (peek(1).is(TokenType::COLON)) {
      parseLabeledStatement();
      return nullptr;
    }
    // fall through
  }
    // fall through
  default:
    const Token *start = &peek();
    Expression *exp = parseExpression();
    expect(TokenType::SEMICOLON, __FUNCTION__);
    return new ExpressionStatement(idx++, start, exp);
  }
  return nullptr;
}

void FastParser::parseLabeledStatement() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  expect(TokenType::IDENTIFIER);
  expect(TokenType::COLON);
  parseStatement();
}

Statement *FastParser::parseSelectionStatement() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  const Token *token = &peek();
  expect(TokenType::IF);
  expect(TokenType::PARENTHESIS_OPEN, __FUNCTION__);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE, __FUNCTION__);
  Statement *if_stat = parseStatement();
  if (!if_stat->instanceof <CompoundStatement>())
    if_stat = new CompoundStatement(idx++, token, {if_stat});
  if (peek().is(TokenType::ELSE) && consume(__FUNCTION__)) {
    token = &peek(-1);
    Statement *else_stat = parseStatement();
    if (!else_stat->instanceof <CompoundStatement>())
      else_stat = new CompoundStatement(idx++, token, {else_stat});
    return new IfElseStatement(idx++, token, cond, if_stat, else_stat);
  } else {
    return new IfElseStatement(idx++, token, cond, if_stat);
  }
}

Statement *FastParser::parseIterationStatement() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  const Token *token = &peek();
  expect(TokenType::WHILE);
  expect(TokenType::PARENTHESIS_OPEN);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  Statement *block = parseStatement();
  if (!block->instanceof <CompoundStatement>())
    block = new CompoundStatement(idx++, token, {block});
  return new WhileStatement(idx++, token, cond, block);
}

void FastParser::parseDeclarations() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      consume(__FUNCTION__);
      parseParameterList();
      // fall through
    } else {
      parseExpression();
      return;
    }
    // fall through
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    consume(__FUNCTION__);
    if (peek().is_not(TokenType::BRACE_OPEN))
      return;
    // fall through
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  auto exp = parseUnaryExpression();
  if (peek().is_oneof(BINARY_OP)) {
    if (this->debug) {
      std::cout << "\033[0;37m" << peek() << "\033[0m -> "
                << "parseBinaryExpression" << std::endl;
    }
    consume(__FUNCTION__);
    switch (peek(-1).getType()) {
    case TokenType::DIV:
    case TokenType::STAR:
      return new MultiplicativeExpression(idx++, exp, &peek(-1),
                                          parseExpression());
    case TokenType::PLUS:
    case TokenType::MINUS:
      return new AdditiveExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::LESS:
      return new RelationalExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
      return new EqualityExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::AND:
      return new LogicalAndExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::OR:
      return new LogicalOrExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::ASSIGN:
      return new AssignmentExpression(idx++, exp, &peek(-1), parseExpression());
    case TokenType::PLUS_ASSIGN:
      return new AssignmentExpression(idx++, exp, &peek(-1), parseExpression());
    default:
      return nullptr;
    }
  } else
    return exp;
}

Expression *FastParser::parseUnaryExpression() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
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
  default:
    return exp;
  }
}

Expression *FastParser::parsePrimaryExpression() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    consume(__FUNCTION__);
    return new PrimaryExpression(idx++, &peek(-1));
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
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  // TODO implement
}
ASTNode *FastParser::parseTranslationUnit() {
  if (this->debug) {
    print_token();
    std::cout << "\033[0;37m" << peek() << "\033[0m -> " << __FUNCTION__
              << std::endl;
  }
  return new Ghost(idx++, &peek(), parseExternalDeclaration());
}

} // namespace ccc
