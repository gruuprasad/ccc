#include "fast_parser.hpp"

namespace ccc {

// (6.9) translationUnit :: external-declaration+
void FastParser::parseTranslationUnit() {
  while (peek().is_not(TokenType::TOKENEND)) {
    parseExternalDeclaration();
  }
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto declaration (see below),
// hence we parse upto that, then find out which type of AST node to be created.
void FastParser::parseExternalDeclaration() {
    parseFuncDefOrDeclaration();
}

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt) compound-statement
// (6.7)  declaration :: type-specifier declarator(opt) ;
void FastParser::parseFuncDefOrDeclaration() {
  // Presence or absence of SEMICOLON determines whether declaration or function-definition
  parseTypeSpecifier();
  if (mayExpect(TokenType::SEMICOLON))  {
    return;
  }
  parseDeclarator();
  if (peek().is(TokenType::SEMICOLON)) {
    nextToken();
    return;
  }

  // Function definition
  while (peek().is(C_TYPES)) {
    parseDeclaration();
  }
  parseCompoundStatement();
  return;
}

// (6.7.2) type-specifier :: void | char | short | int | struct-or-union-specifier
void FastParser::parseTypeSpecifier() {
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
                             "\", mustExpecting type specifier");
  }
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list )
// (6.7.6) abstract-declarator :: pointer | pointer(opt) direct-abstract-declarator
// (6.7.6) direct-abstract-declarator :: ( abstract-declarator ) | ( parameter-list(opt) )+
void FastParser::parseDeclarator() {
  int ptrCount = 0;                       // > 0 means declarator is ptr type
  bool existIdent = false;                // decides whether declarator is abstract or not.
  parseList( [&]() { ++ptrCount; }, TokenType::STAR); // consume  0 or more pointers
  if (peek().is(TokenType::IDENTIFIER)) {
    nextToken();
    existIdent = true;
  }
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    if (peek().is(C_TYPES)) {
      parseParenthesizedFn([&] () { parseParameterList(); });
      return;                           // based on existIdent value create AST.
    }
    parseParenthesizedFn([&] () { parseDeclarator(); });
    return;                             // based on existIdent value create AST.
  }

  if (existIdent) {
    if (ptrCount != 0) {
      // pointer-type
      return;
    }

    // non-pointer declarator type.
    return;
  }

  if (ptrCount != 0) {
    // abstract-pointer type.
    return;
  }

  error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
      "Unexpected Token: \"" + peek().name() +
      "\", expecting declarator");
}

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
void FastParser::parseParameterList() {
  parseList(
      [&]() {
      parseParameterDeclaration();
      },
      TokenType::COMMA);
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier abstract-declarator(opt)
void FastParser::parseParameterDeclaration() {
  parseTypeSpecifier();
  if (peek().is(TokenType::COMMA)) return;
  if (peek().is(TokenType::PARENTHESIS_CLOSE)) return;
  parseDeclarator();
}

// (6.7)  declaration :: type-specifier declarator(opt) ;
void FastParser::parseDeclaration() {
  parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    parseDeclarator();
  mustExpect(TokenType::SEMICOLON);
}

void FastParser::parseDeclarations() {
  do {
    parseDeclaration();
  } while (peek().is(C_TYPES));
}

// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) { struct-declaration+ } | struct identifier
void FastParser::parseStructOrUnionSpecifier() {
  mustExpect(TokenType::STRUCT);
  if (peek().is(TokenType::IDENTIFIER)) {
    nextToken();
    if (peek().is_not(TokenType::BRACE_OPEN))
      return;
  }
  
  if (peek().is(TokenType::BRACE_OPEN)) {
    nextToken();
    do {
      parseStructDeclaration();
    } while (peek().is_not(TokenType::BRACE_CLOSE));
    mustExpect(TokenType::BRACE_CLOSE);
    return;
  }
  error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
      "Unexpected Token: \"" + peek().name() +
      "\", expecting struct");
}

// (6.7.2.1) struct-declaration :: type-specifier declarator (opt) ;
void FastParser::parseStructDeclaration() {
  parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  mustExpect(TokenType::SEMICOLON);
}

void FastParser::parseCompoundStatement() {
  mustExpect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is(C_TYPES)) {
      parseTypeSpecifier();
      parseDeclarator();
      mustExpect(TokenType::SEMICOLON);
    } else {
      parseStatement();
    }
    if (fail())
      return;
  }
  mustExpect(TokenType::BRACE_CLOSE);
}

void FastParser::parseStatement() {
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
    nextToken();
    mustExpect(TokenType::IDENTIFIER);
    break;
  case TokenType::CONTINUE:
    nextToken();
    break;
  case TokenType::BREAK:
    nextToken();
    break;
  case TokenType::RETURN:
    nextToken();
    if (peek().is_not(TokenType::SEMICOLON))
      parseExpression();
    break;
  default:
    if (peek().is_not(TokenType::SEMICOLON))
      parseExpression();
    break;
  }
  mustExpect(TokenType::SEMICOLON);
}

void FastParser::parseLabeledStatement() {
  mustExpect(TokenType::IDENTIFIER);
  mustExpect(TokenType::COLON);
  parseStatement();
}

void FastParser::parseSelectionStatement() {
  mustExpect(TokenType::IF);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  peek().is(TokenType::IF) ? parseSelectionStatement() : parseStatement();
  if (mayExpect(TokenType::ELSE))
    parseStatement();
  return;
}

void FastParser::parseIterationStatement() {
  mustExpect(TokenType::WHILE);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
}

// Expressions
// (6.5.17) expression: assignment+ (comma-separated)
// (6.5.16) assignment: conditional | unary assignment-op assignment
// (6.5.16) assignment-op: = | *= | /= | %= | += | -= | <<= | >>= | &= | ^= |
// |= (6.5.15) conditional: logical-OR | logical-OR ? expression : conditional
// (6.5.14) logical-OR: logical-AND | logical-OR || logical-AND
// (6.5.13) logical-AND: inclusive-OR | logical-AND && inclusive-OR
// (6.5.12) inclusive-OR: exclusive-OR | inclusive-OR | exclusive-OR
// (6.5.11) exclusive-OR: AND | exclusive-OR ^ AND
// (6.5.10) AND: equality | AND & equality
// (6.5.9) equality: relational | equality == relational | equality !=
// relational 
// (6.5.8) relational: shift | relational < shift | relational >
// shift | relational <= shift | relational >= shift 
// (6.5.7) shift: additive | shift << additive | shift >> additive (6.5.6) additive: multiplicative |
// additive + multiplicative | additive - multiplicative (6.5.5)
// multiplicative: cast | multiplicative * cast | multiplicative / cast |
// multiplicative % cast (6.5.4) cast: unary | ( type-name ) cast (6.5.3)
// unary: postfix | ++ unary | -- unary | unary-op cast | sizeof unary |
// sizeof ( type-name ) (6.5.3) unary-op: & | * | + | - | ~ |  ! (6.5.2)
// postfix: primary | postfix [ expression ] | postfix (
// argument-expr-list(opt) ) | postfix . identifier
//                  postfix -> identifier | postfix -- | ( type-name ) {
//                  initializer-list } | ( type-name ) { initializer-list , }
// (6.5.2) argument-expr-list: assignment | argument-expr-list , assignment
// (6.5.1) primary: identifer | constant | string-literal | ( expression )

// Expressions
// 
void FastParser::parseExpression() {
  parseUnaryExpression();

  while (true) {
    // Parse binary op and RHS, if there is.
    if (peek().is(BINARY_OP)) {
      parseUnaryExpression();
    } else {
      break;
    }
  }
}

void FastParser::parseUnaryExpression() {
  if (peek().is(UNARY_OP)) {
    if (mayExpect(TokenType::PARENTHESIS_OPEN)) {
      parseTypeSpecifier();
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      return;
    }
    parseUnaryExpression();
  } else {
    parsePostfixExpression();
  }
}

void FastParser::parsePostfixExpression() {
  parsePrimaryExpression();
  // optional postfix varients
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN:
    nextToken();
    parseExpression();
    mustExpect(TokenType::BRACKET_CLOSE);
    return;
  case TokenType::PARENTHESIS_OPEN:
    nextToken();
    parseArgumentExpressionList();
    mayExpect(TokenType::PARENTHESIS_CLOSE);
    return;
  case TokenType::DOT:
    nextToken();
    mayExpect(TokenType::IDENTIFIER);
    return;
  case TokenType::ARROW:
    nextToken();
    mayExpect(TokenType::IDENTIFIER);
    return;
  default:
    return;
  }
}

void FastParser::parsePrimaryExpression() {
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
  case TokenType::NUMBER:
  case TokenType::CHARACTER:
  case TokenType::STRING:
    nextToken();
    return;
  case TokenType::PARENTHESIS_OPEN:
    nextToken();
    parseExpression();
    mayExpect(TokenType::PARENTHESIS_CLOSE);
    return;
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", in primary expression.");
  }
}

void FastParser::parseArgumentExpressionList() {
  // TODO implement
}

} // namespace ccc
