#include "fast_parser.hpp"

namespace ccc {

// (6.9) translationUnit :: external-declaration+
void FastParser::parseTranslationUnit() {
  while (peek().is_not(TokenType::ENDOFFILE)) {
    parseExternalDeclaration();
  }
  return;
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto
// declaration (see below), hence we parse upto that, then find out which type
// of AST node to be created.
void FastParser::parseExternalDeclaration() {
  return parseFuncDefOrDeclaration();
}

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement (6.7)  declaration :: type-specifier declarator(opt) ;
// (6.7.2) type-specifier :: void | char | short | int |
// struct-or-union-specifier
void FastParser::parseFuncDefOrDeclaration() {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  if (peek().is(SCALAR_TYPES)) {
    nextToken();
  } else if (peek().is(TokenType::STRUCT)) {
    parseStructTypeDeclaration();
  } else {
    parser_error(peek());
    return;
  }

  if (peek().is(TokenType::BRACE_OPEN)) {
    parseStructDefinition();
    if (peek().is_not(TokenType::SEMICOLON)) {
      parseDeclarator();
      mustExpect(TokenType::SEMICOLON);
      return;
    }
  }

  if (mayExpect(TokenType::SEMICOLON)) {
      return;
  }

  parseDeclarator();
  if (fail()) {
    return;
  }

  if (mayExpect(TokenType::SEMICOLON)) {
    return;
  }

  // Function definition
  while (peek().is(C_TYPES)) {
    parseDeclaration();
  }
  if (peek().is(TokenType::BRACE_OPEN)) {
    parseCompoundStatement();
    return; 
  }

  parser_error(peek());
  return;
}

// (6.7.2.1) struct-or-union-specifier :: struct identifier
void FastParser::parseStructTypeDeclaration() {
  nextToken();
  if (peek().is(TokenType::IDENTIFIER)) {
    nextToken();
    return;
  }
  return;
}

// struct body parsing
// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) {
// struct-declaration+ }
void FastParser::parseStructDefinition() {
  mustExpect(TokenType::BRACE_OPEN);
  do {
    parseStructMemberDeclaration();
    if (fail()) 
      return;
  } while (peek().is_not(TokenType::BRACE_CLOSE));

  if (mustExpect(TokenType::BRACE_CLOSE)) {
    return;
  }

  parser_error(peek());
  return;
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list ) 
// (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | ( parameter-list(opt) )+
void FastParser::parseDeclarator() {
  if (peek().is(TokenType::STAR)) {
    // Parse Pointer (*) symbols
    parseList([&]() { ++ptrCount; },
              TokenType::STAR);
  }

  // Parenthesized declarator
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    nextToken(); // consume '('
    parseDeclarator();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    if (ptrCount != 0) {
      return;
    }
    return;
  }

  if (peek().is(TokenType::IDENTIFIER)) {
    nextToken();
    if (ptrCount != 0) {
      ;
    }

    // Function type declarator magic happens here
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      mustExpect(TokenType::PARENTHESIS_OPEN);
      if (peek().is(C_TYPES)) {
        parseParameterList();
      }
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      if (!fail()) {
        return;
      }
      return;
    }
    return;
  }

  // TODO abstract-pointer type, learn more about abstract type.
  // TODO Abstract parameter-list

  return;
}

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
void FastParser::parseParameterList() {
  do {
    parseParameterDeclaration();
    if (fail()) {
      return;
    }
  } while (mayExpect(TokenType::COMMA));
  return;
}

void FastParser::parseTypeSpecifier() {
  if (peek().is(SCALAR_TYPES)) {
    return;
  } else if (peek().is(TokenType::STRUCT)) {
    return parseStructTypeDeclaration();
  }
  parser_error(peek(), "type specifier");
  return;
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
void FastParser::parseParameterDeclaration() {
  parseTypeSpecifier();
  if (peek().is(TokenType::COMMA) || peek().is(TokenType::PARENTHESIS_CLOSE))
    return;    // Abstract-declaration
  parseDeclarator();
  if (fail()) {
    return;
  }
  return;
}

// (6.7)  declaration :: type-specifier declarator(opt) ;
void FastParser::parseDeclaration() {
  parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    parseDeclarator();
  mustExpect(TokenType::SEMICOLON);
  return;
}

// (6.7.2.1) struct-declaration :: type-specifier declarator (opt) ;
void FastParser::parseStructMemberDeclaration() {
  // XXX support for only one member per declaration in struct:int a; not int a, b; 
  parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON)) {
    parseDeclarator();
  }
  mustExpect(TokenType::SEMICOLON);
  if (fail()) {
    return;
  }
  return;
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
    if (fail()) {
      return;
    }
  }
  mustExpect(TokenType::BRACE_CLOSE);
  return;
}

void FastParser::parseStatement() {
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    return parseLabeledStatement();
  case TokenType::BRACE_OPEN:
    return parseCompoundStatement();
  case TokenType::IF:
    return parseSelectionStatement();
  case TokenType::WHILE:
    return parseIterationStatement();
  case TokenType::GOTO:
    nextToken();
    if (peek().is(TokenType::IDENTIFIER)) {
      nextToken();
      return;
    }
    parser_error(peek());
    return;
  case TokenType::CONTINUE:
    nextToken();
    break;
  case TokenType::BREAK:
    nextToken();
    break;
  case TokenType::RETURN:
    nextToken();
    if (peek().is_not(TokenType::SEMICOLON)) {
      expr = parseExpression();
    }
    break;
  default:
    peek();
    if (peek().is_not(TokenType::SEMICOLON)) {
      parseExpression();
    }
    break;
  }
  mustExpect(TokenType::SEMICOLON);
  return;
}

void FastParser::parseLabeledStatement() {
  nextToken();
  if (mustExpect(TokenType::COLON)) {
    parseStatement();
  }
  return;
}

void FastParser::parseSelectionStatement() {
  mustExpect(TokenType::IF);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  if (fail()) {
    return;
  }

  parseStatement();
  if (!fail() && mayExpect(TokenType::ELSE)) {
    parseStatement();
  }

  return;
}

void FastParser::parseIterationStatement() {
  mustExpect(TokenType::WHILE);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  parseStatement();
  return;
}

// Expressions
// (6.5.17) expression: assignment
void FastParser::parseExpression() {
  return parseAssignmentExpression();
}

// (6.5.16) assignment-expr: conditional-expr | unary-expr assignment-op
// assignment-expr (6.5.15) conditional-expr: logical-OR | logical-OR ?
// expression : conditional-expr
void FastParser::parseAssignmentExpression() {
  parseUnaryExpression();               // LHS
  parseBinOpWithRHS(std::move(lhs), 1); // BinOpLeft + RHS
  return;
}

void FastParser::parseBinOpWithRHS(Precedence minPrec) {
  auto nextTokenPrec = peek().getPrecedence();

  while (true) {
    // If precedence of BinOp encounted is smaller than current Precedence
    // level return LHS.
    if (nextTokenPrec < minPrec)
      return; // LHS

    auto binOpLeft = nextToken();

    // Handle conditional operator middle expression
    if (binOpLeft.getType() == TokenType::CONDITIONAL) {
      parseExpression(); // Ternary middle
      mustExpect(TokenType::COLON);
    }

    parseUnaryExpression(); // RHS

    auto binOpLeftPrec = nextTokenPrec;
    nextTokenPrec = peek().getPrecedence(); // binOpRight

    if (binOpLeftPrec < nextTokenPrec) {
      // Evaluate RHS + binOpRight...
      parseBinOpWithRHS(binOpLeftPrec); // new RHS after evaluation
    }

    // Make AST LHS = LHS + RHS or lhs + ternary_middle + rhs
    bool ternary_middle = false;
    if (ternary_middle) {
      ;
    } else {
      ;
    }
    nextTokenPrec = peek().getPrecedence();
  }
}

// (6.5.3) unary-expression : postfix-expression
//                            unary-operator unary-expression
//                            sizeof unary-expression
//                            sizeof unary-expression
void FastParser::parseUnaryExpression() {
  Token op;

  if (peek().is(UNARY_OP)) {
    op = nextToken();
    parseUnaryExpression();
    return;
  }

  if (peek().is(TokenType::SIZEOF)) {
    op = nextToken();
    if (mayExpect(TokenType::PARENTHESIS_OPEN)) {
      // FIXME Parsing type-expression
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      return;
    }

    parseUnaryExpression();
    return;
  }
  return parsePostfixExpression();
}

// (6.5.2) postfix-expression : primary-expr
//                              postfix-expr [ expression ]
//                              postfix-expr ( argument-expr-list(opt) )
//                              postfix-expr . identifier
//                              postfix-expr -> identifier
void FastParser::parsePostfixExpression() {
  parsePrimaryExpression();
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN:
    op = nextToken();
    parseExpression();
    mustExpect(TokenType::BRACKET_CLOSE);
    return;
  case TokenType::PARENTHESIS_OPEN:
    op = nextToken();
    parseArgumentExpressionList();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return;
  case TokenType::DOT:
  case TokenType::ARROW:
    op = nextToken();
    if (peek().is(TokenType::IDENTIFIER)) {
      nextToken();
      return;
    }
    parser_error(peek());
    return;
  default:
    return;
  }
}

// (6.5.1) primary: identifer | constant | string-literal | ( expression )
void FastParser::parsePrimaryExpression() {
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    nextToken();
    return;
  case TokenType::NUMBER:
    nextToken();
    return;
  case TokenType::CHARACTER:
    nextToken();
    return; // XXX CharacterExpression AST necessary?
  case TokenType::STRING:
    nextToken();
    return;
  case TokenType::PARENTHESIS_OPEN:
    mustExpect(TokenType::PARENTHESIS_OPEN);
    parseExpression();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return;
  default:
    parser_error(peek());
    return;
  }
}

void FastParser::parseArgumentExpressionList() {
  // TODO implement
  return;
}

} // namespace ccc
