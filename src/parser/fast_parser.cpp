#include "fast_parser.hpp"
#include <ast/ast_node.hpp>
#include <ast/statement.hpp>

namespace ccc {

ASTNode *FastParser::parseExternalDeclaration() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  while (curTokenIdx + 1 != tokens.size())
    parseFuncDefOrDeclaration();
  return new Declaration(count++, nullptr, TypeSpecifier::VOID);
}

ASTNode *FastParser::parseFuncDefOrDeclaration() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  parseTypeSpecifiers();
  if (peek().is(TokenType::SEMICOLON) && consume())
    return nullptr;
  parseDeclarator();
  switch (peek().getType()) {
  case TokenType::SEMICOLON:
    consume();
    return nullptr;
  case TokenType::BRACE_OPEN:
    parseCompoundStatement();
    return nullptr;
  default:
    parseDeclarations();
    parseCompoundStatement();
    return nullptr;
  }
}

Statement *FastParser::parseCompoundStatement() {
  std::vector<ASTNode *> block;
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  expect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is_oneof(C_TYPES)) {
      parseTypeSpecifiers();
      parseDeclarator();
      expect(TokenType::SEMICOLON);
    } else {
      block.push_back(parseStatement());
    }
    if (fail())
      return nullptr;
  }
  consume();
  return new CompoundStatement(count++, block);
}

void FastParser::parseBlockItemList() {}

Statement *FastParser::parseStatement() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::SEMICOLON: {
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
  case TokenType::IDENTIFIER: {
    if (peek(1).is(TokenType::COLON)) {
      parseLabeledStatement();
      return nullptr;
    }
  }
  default:
    Expression *exp = parseExpression();
    expect(TokenType::SEMICOLON);
    return new ExpressionStatement(count++, exp);
  }
  expect(TokenType::SEMICOLON);
}

void FastParser::parseLabeledStatement() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  expect(TokenType::IDENTIFIER, TokenType::COLON);
  parseStatement();
}

void FastParser::parseSelectionStatement() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  expect(TokenType::IF, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  if (peek().is(TokenType::IF))
    parseSelectionStatement();
  else
    parseStatement();
  if (peek().is(TokenType::ELSE) && consume())
    parseStatement();
  return;
}

void FastParser::parseIterationStatement() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  expect(TokenType::WHILE, TokenType::PARENTHESIS_OPEN);
  parseExpression();
  expect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
}

void FastParser::parseDeclarations() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  do {
    parseTypeSpecifiers();
    if (peek().is_not(TokenType::SEMICOLON))
      parseDeclarator();
    expect(TokenType::SEMICOLON);
  } while (peek().is_oneof(C_TYPES));
}

void FastParser::parseTypeSpecifiers() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::VOID:
  case TokenType::CHAR:
  case TokenType::SHORT:
  case TokenType::INT:
    nextToken();
    return;
  case TokenType::STRUCT:
    nextToken();
    return parseStructOrUnionSpecifier();
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting type specifier");
  }
}

void FastParser::parseDeclarator() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  parseList([&]() {}, TokenType::STAR);
  parseDirectDeclarator();
}

void FastParser::parseDirectDeclarator() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      consume();
      parseParameterList();
    } else {
      parseExpression();
      return;
    }
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
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  parseList(
      [&]() {
        parseTypeSpecifiers();
        parseDeclarator();
      },
      TokenType::COMMA);
  expect(TokenType::PARENTHESIS_CLOSE);
}

void FastParser::parseStructOrUnionSpecifier() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
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
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  parseTypeSpecifiers();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  expect(TokenType::SEMICOLON);
}

// Expressions
Expression *FastParser::parseExpression() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  auto exp = parseUnaryExpression();
  if (peek().is_oneof(BINARY_OP)) {
    std::cout << peek() << " -> "
              << "parseBinaryExpression" << std::endl;
    switch (peek().getType()) {
    case TokenType::STAR:
      consume();
      return new MultiplicativeExpression(count++, exp, parseUnaryExpression());
    case TokenType::PLUS:
    case TokenType::MINUS:
      consume();
      return new AdditiveExpression(count++, exp, parseUnaryExpression());
    case TokenType::LESS:
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
      consume();
      return new RelationalExpression(count++, exp, parseUnaryExpression());
    case TokenType::AND:
      consume();
      return new LogicalAndExpression(count++, exp, parseUnaryExpression());
    case TokenType::OR:
      consume();
      return new LogicalOrExpression(count++, exp, parseUnaryExpression());
    case TokenType::ASSIGN:
      consume();
      return new AssignmentExpression(count++, exp, parseUnaryExpression());
    case TokenType::PLUS_ASSIGN:
      consume();
      return new AssignmentExpression(count++, exp, parseUnaryExpression());
    }
  } else
    return exp;
}

Expression *FastParser::parseUnaryExpression() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
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
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
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
  }
  return exp;
}

Expression *FastParser::parsePrimaryExpression() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    consume();
    return new PrimaryExpression(count++);
  case TokenType::PARENTHESIS_OPEN:
    consume();
    parseExpression();
    expect(TokenType::PARENTHESIS_CLOSE);
    return new PrimaryExpression(count++);
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", in primary expression.");
  }
}

void FastParser::parseArgumentExpressionList() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  // TODO implement
}
ASTNode *FastParser::parseTranslationUnit() {
  std::cout << peek() << " -> " << __FUNCTION__ << std::endl;
  std::vector<ASTNode *> items;
  items.push_back(parseExternalDeclaration());
  return new CompoundStatement(count++, items);
}

} // namespace ccc
