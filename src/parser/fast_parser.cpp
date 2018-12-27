#include "fast_parser.hpp"

namespace ccc {

ASTNode *FastParser::parseTranslationUnit() {
#if DEBUG
  printParserTrace();
#endif
  return parseExternalDeclaration();
}

ASTNode *FastParser::parseExternalDeclaration() {
#if DEBUG
  printParserTrace();
#endif
  std::vector<ASTNode *> block;
  const Token *start = &peek();
  while (!peek().is(TokenType::ENDOFFILE))
    block.push_back(parseFuncDefOrDeclaration());
  return new Ghost(start, block);
}

ASTNode *FastParser::parseFuncDefOrDeclaration() {
#if DEBUG
  printParserTrace();
#endif
  if (peek().is_oneof(C_TYPES)) {
    parseTypeSpecifiers();
    if (peek().is(TokenType::SEMICOLON) && consume())
      return nullptr;
    else {
      parseDeclarator();
      expect(TokenType::SEMICOLON);
    }
  } else {
    switch (peek().getType()) {
    case TokenType::SEMICOLON:
      consume();
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
#if DEBUG
  printParserTrace();
#endif
  std::vector<ASTNode *> block;
  const Token *brace = &peek();
  expect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      block.push_back(new Ghost(&peek()));
      expect(TokenType::SEMICOLON);
    } else {
      block.push_back(parseStatement());
    }
  }
  expect(TokenType::BRACE_CLOSE);
  return new CompoundStatement(brace, block);
}

Statement *FastParser::parseBlockItemList() { return nullptr; }

Statement *FastParser::parseStatement() {
#if DEBUG
  printParserTrace();
#endif
  switch (peek().getType()) {
  case TokenType::SEMICOLON: {
    expect(TokenType::SEMICOLON);
    return nullptr;
  }
  case TokenType::BRACE_OPEN:
    return parseCompoundStatement();
  case TokenType::IF:
    return parseSelectionStatement();
  case TokenType::WHILE:
    return parseIterationStatement();
  case TokenType::GOTO:
    consume();
    expect(TokenType::IDENTIFIER);
    expect(TokenType::SEMICOLON);
    break;
  case TokenType::CONTINUE: {
    const Token *token = &peek();
    consume();
    expect(TokenType::SEMICOLON);
    return new ContinueStatement(token);
  }
  case TokenType::BREAK: {
    const Token *token = &peek();
    consume();
    expect(TokenType::SEMICOLON);
    return new BreakStatement(token);
  }
  case TokenType::RETURN: {
    const Token *token = &peek();
    consume();
    if (peek().is_not(TokenType::SEMICOLON)) {
      Expression *exp = parseExpression();
      expect(TokenType::SEMICOLON);
      return new ReturnStatement(token, exp);
    }
    consume();
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
    expect(TokenType::SEMICOLON);
    return new ExpressionStatement(start, exp);
  }
  return nullptr;
}

Statement *FastParser::parseLabeledStatement() {
#if DEBUG
  printParserTrace();
#endif
  Expression *ident = parseExpression();
  expect(TokenType::COLON);
  return new LabelStatement(ident, parseStatement());
}

Statement *FastParser::parseSelectionStatement() {
#if DEBUG
  printParserTrace();
#endif
  const Token *token = &peek();
  expect(TokenType::IF);
  expect(TokenType::PARENTHESIS_OPEN);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  Statement *if_stat;
  if (peek().is(TokenType::BRACE_OPEN))
    if_stat = parseCompoundStatement();
  else
    if_stat = new CompoundStatement(token, {parseStatement()});
  if (peek().is(TokenType::ELSE) && consume()) {
    token = &peek(-1);
    if (peek().is(TokenType::BRACE_OPEN))
      return new IfElseStatement(token, cond, if_stat,
                                 parseCompoundStatement());
    else
      return new IfElseStatement(
          token, cond, if_stat,
          new CompoundStatement(token, {parseStatement()}));
  } else {
    return new IfElseStatement(token, cond, if_stat);
  }
}

Statement *FastParser::parseIterationStatement() {
#if DEBUG
  printParserTrace();
#endif
  const Token *token = &peek();
  expect(TokenType::WHILE);
  expect(TokenType::PARENTHESIS_OPEN);
  Expression *cond = parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  if (peek().is(TokenType::BRACE_OPEN))
    return new WhileStatement(token, cond, parseCompoundStatement());
  else
    return new WhileStatement(token, cond,
                              new CompoundStatement(token, {parseStatement()}));
}

void FastParser::parseDeclarations() {
#if DEBUG
  printParserTrace();
#endif
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON);
  } while (peek().is_oneof(C_TYPES));
}

void FastParser::parseTypeSpecifiers() {
#if DEBUG
  printParserTrace();
#endif
  switch (peek().getType()) {
  case TokenType::VOID:
  case TokenType::CHAR:
  case TokenType::SHORT:
  case TokenType::INT:
    consume();
    return;
  case TokenType::STRUCT:
    consume();
    return parseStructOrUnionSpecifier();
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting type specifier");
  }
}

void FastParser::parseDeclarator() {
#if DEBUG
  printParserTrace();
#endif
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
#if DEBUG
  printParserTrace();
#endif
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      consume();
      parseParameterList();
      // fall through
    } else {
      parseExpression();
      return;
    }
    // fall through
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
} // namespace ccc

void FastParser::parseParameterList() {
#if DEBUG
  printParserTrace();
#endif
  parseList(
      [&]() {
        parseTypeSpecifiers();
        parseDeclarator();
      },
      TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
}

void FastParser::parseStructOrUnionSpecifier() {
#if DEBUG
  printParserTrace();
#endif
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    consume();
    if (peek().is_not(TokenType::BRACE_OPEN))
      return;
    // fall through
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
#if DEBUG
  printParserTrace();
#endif
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON);
}

// Expressions
Expression *FastParser::parseExpression() {
#if DEBUG
  printParserTrace();
#endif
  auto exp = parseUnaryExpression();
  if (peek().is_oneof(BINARY_OP)) {
    return new BinaryExpression(exp, pop(), parseExpression());
  } else
    return exp;
} // namespace ccc

Expression *FastParser::parseUnaryExpression() {
#if DEBUG
  printParserTrace();
#endif
  if (peek().is_oneof(UNARY_OP) && consume()) {
    if (peek().is(TokenType::PARENTHESIS_OPEN) && consume()) {
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
#if DEBUG
  printParserTrace();
#endif
  Expression *exp = parsePrimaryExpression();
  // optional postfix varients
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN: {
    consume();
    Expression *exp = parseExpression();
    expect(TokenType::BRACKET_CLOSE);
    return exp;
  }
  case TokenType::PARENTHESIS_OPEN:
    consume();
    parseArgumentExpressionList();
    expect(TokenType::PARENTHESIS_CLOSE);
    return nullptr;
  case TokenType::DOT:
    consume();
    expect(TokenType::IDENTIFIER);
    return nullptr;
  case TokenType::ARROW:
    consume();
    expect(TokenType::IDENTIFIER);
    return nullptr;
  default:
    return exp;
  }
}

Expression *FastParser::parsePrimaryExpression() {
#if DEBUG
  printParserTrace();
#endif
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    consume();
    return new PrimaryExpression(&peek(-1));
  case TokenType::PARENTHESIS_OPEN: {
    consume();
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
#if DEBUG
  printParserTrace();
#endif
  // TODO implement
}

} // namespace ccc
