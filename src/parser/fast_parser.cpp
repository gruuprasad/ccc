#include "fast_parser.hpp"

namespace ccc {

ASTNode *FastParser::parseTranslationUnit() {
  printParserTrace();
  return parseExternalDeclaration();
}

ASTNode *FastParser::parseExternalDeclaration() {
  printParserTrace();
  std::vector<ASTNode *> block;
  const Token *start = &peek();
  while (!peek().is(TokenType::ENDOFFILE))
    block.push_back(parseFuncDefOrDeclaration());
  return new Ghost(start, block);
}

ASTNode *FastParser::parseFuncDefOrDeclaration() {
  printParserTrace();
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

Statement *FastParser::parseCompoundStatement() {
  printParserTrace();
  std::vector<ASTNode *> block;
  const Token *brace = &peek();
  expect(TokenType::BRACE_OPEN, __FUNCTION__);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      block.push_back(new Ghost(&peek()));
      expect(TokenType::SEMICOLON, __FUNCTION__);
    } else {
      block.push_back(parseStatement());
    }
  }
  expect(TokenType::BRACE_CLOSE, __FUNCTION__);
  return new CompoundStatement(brace, block);
}

Statement *FastParser::parseBlockItemList() { return nullptr; }

Statement *FastParser::parseStatement() {
  printParserTrace();
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
    return new ContinueStatement(pop(__FUNCTION__));
  case TokenType::BREAK:
    return new BreakStatement(pop(__FUNCTION__));
  case TokenType::RETURN: {
    const Token *token = &peek();
    consume(__FUNCTION__);
    if (peek().is_not(TokenType::SEMICOLON)) {
      Expression *exp = parseExpression();
      expect(TokenType::SEMICOLON, __FUNCTION__);
      return new ReturnStatement(token, exp);
    }
    consume(__FUNCTION__);
    return new ReturnStatement(token);
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
    return new ExpressionStatement(start, exp);
  }
  return nullptr;
}

Statement *FastParser::parseLabeledStatement() {
  printParserTrace();
  Expression *ident = parseExpression();
  expect(TokenType::COLON);
  return new LabelStatement(ident, parseStatement());
}

Statement *FastParser::parseSelectionStatement() {
  printParserTrace();
  const Token *token = &peek();
  expect(TokenType::IF);
  expect(TokenType::PARENTHESIS_OPEN, __FUNCTION__);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE, __FUNCTION__);
  Statement *if_stat = parseStatement();
  if (!if_stat->instanceof <CompoundStatement>())
    if_stat = new CompoundStatement(token, {if_stat});
  if (peek().is(TokenType::ELSE) && consume(__FUNCTION__)) {
    token = &peek(-1);
    Statement *else_stat = parseStatement();
    if (!else_stat->instanceof <CompoundStatement>())
      else_stat = new CompoundStatement(token, {else_stat});
    return new IfElseStatement(token, cond, if_stat, else_stat);
  } else {
    return new IfElseStatement(token, cond, if_stat);
  }
}

Statement *FastParser::parseIterationStatement() {
  printParserTrace();
  const Token *token = &peek();
  expect(TokenType::WHILE);
  expect(TokenType::PARENTHESIS_OPEN);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  Statement *block = parseStatement();
  if (!block->instanceof <CompoundStatement>())
    block = new CompoundStatement(token, {block});
  return new WhileStatement(token, cond, block);
}

void FastParser::parseDeclarations() {
  printParserTrace();
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON, __FUNCTION__);
  } while (peek().is_oneof(C_TYPES));
}

void FastParser::parseTypeSpecifiers() {
  printParserTrace();
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
  printParserTrace();
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  printParserTrace();
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
  printParserTrace();
  parseList(
      [&]() {
        parseTypeSpecifiers();
        parseDeclarator();
      },
      TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
}

void FastParser::parseStructOrUnionSpecifier() {
  printParserTrace();
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
  printParserTrace();
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON, __FUNCTION__);
}

// Expressions
Expression *FastParser::parseExpression() {
  printParserTrace();
  auto exp = parseUnaryExpression();
  if (peek().is_oneof(BINARY_OP)) {
    switch (peek().getType()) {
    case TokenType::DIV:
    case TokenType::STAR:
    case TokenType::PLUS:
    case TokenType::MINUS:
    case TokenType::LESS:
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::AND:
    case TokenType::OR:
    case TokenType::ASSIGN:
    case TokenType::PLUS_ASSIGN:
      return new BinaryExpression(exp, pop(__FUNCTION__), parseExpression());
    default:
      return nullptr;
    }
  } else
    return exp;
}

Expression *FastParser::parseUnaryExpression() {
  printParserTrace();
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
  printParserTrace();
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
  printParserTrace();
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    consume(__FUNCTION__);
    return new PrimaryExpression(&peek(-1));
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
  printParserTrace();
  // TODO implement
}

void FastParser::print_token() {
  //  for (int i = curTokenIdx; i < tokens.size(); i++) {
  //    std::cout << tokens[i].name();
  //  }
  //  std::cout << std::endl;
}

} // namespace ccc
