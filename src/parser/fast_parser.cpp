#include "fast_parser.hpp"

namespace ccc {

// (6.9) translationUnit :: external-declaration+
void FastParser::parseTranslationUnit() {
  while (peek().is_not(TokenType::ENDOFFILE)) {
    parseExternalDeclaration();
  }
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto
// declaration (see below), hence we parse upto that, then find out which type
// of AST node to be created.
void FastParser::parseExternalDeclaration() { parseFuncDefOrDeclaration(); }

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement (6.7)  declaration :: type-specifier declarator(opt) ;
void FastParser::parseFuncDefOrDeclaration() {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  parseTypeSpecifier();
  if (mayExpect(TokenType::SEMICOLON)) {
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

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator
// ( parameter-list ) (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | ( parameter-list(opt) )+
void FastParser::parseDeclarator() {
  int ptrCount = 0;        // > 0 means declarator is ptr type
  bool existIdent = false; // decides whether declarator is abstract or not.
  parseList([&]() { ++ptrCount; },
            TokenType::STAR); // consume  0 or more pointers
  if (peek().is(TokenType::IDENTIFIER)) {
    nextToken();
    existIdent = true;
  }
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    if (peek().is(C_TYPES)) {
      mustExpect(TokenType::PARENTHESIS_OPEN);
      parseParameterList();
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      return; // based on existIdent value create AST.
    }
    mustExpect(TokenType::PARENTHESIS_OPEN);
    parseDeclarator();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return; // based on existIdent value create AST.
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
  parseList([&]() { parseParameterDeclaration(); }, TokenType::COMMA);
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
void FastParser::parseParameterDeclaration() {
  parseTypeSpecifier();
  if (peek().is(TokenType::COMMA))
    return;
  if (peek().is(TokenType::PARENTHESIS_CLOSE))
    return;
  parseDeclarator();
}

// (6.7)  declaration :: type-specifier declarator(opt) ;
void FastParser::parseDeclaration() {
  parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    parseDeclarator();
  mustExpect(TokenType::SEMICOLON);
}

std::unordered_map<TokenType, TypeSpec> type_map{
    {TokenType::VOID, TypeSpec::VOID},
    {TokenType::CHAR, TypeSpec::CHAR},
    {TokenType::INT, TypeSpec::INT},
    {TokenType::STRUCT, TypeSpec::STRUCT}};

// (6.7.2) type-specifier :: void | char | short | int |
// struct-or-union-specifier
std::unique_ptr<TypeDeclaration> FastParser::parseTypeSpecifier() {
  switch (peek().getType()) {
  case TokenType::VOID:
  case TokenType::CHAR:
  case TokenType::SHORT:
  case TokenType::INT:
    return make_unique<TypeDeclaration>(type_map[nextToken().getType()]);
  case TokenType::STRUCT:
    return parseStructOrUnionSpecifier();
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", mustExpecting type specifier");
  }
}

// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) {
// struct-declaration+ } | struct identifier
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
  std::vector<std::unique_ptr<Statement>> stmt_list;
  mustExpect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is(C_TYPES)) {
      auto type = parseTypeSpecifier();
      auto var = parseDeclarator();
      mustExpect(TokenType::SEMICOLON);
    } else {
      parseStatement();
    }
  }
  mustExpect(TokenType::BRACE_CLOSE);
}

std::unique_ptr<Statement> FastParser::parseStatement() {
  Token src_mark;
  std::unique_ptr<Expression> expr;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    return parseLabeledStatement();
  case TokenType::BRACE_OPEN:
    parseCompoundStatement();
    return std::unique_ptr<CompoundStatement>(); // FIXME
  case TokenType::IF:
    return parseSelectionStatement();
  case TokenType::WHILE:
    return parseIterationStatement();
  case TokenType::GOTO:
    nextToken();
    if (peek().is(TokenType::IDENTIFIER)) {
      return make_unique<GotoStatement>(
          make_unique<IdentifierExpression>(nextToken()));
    }
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting identifer");
    return std::unique_ptr<GotoStatement>();
  case TokenType::CONTINUE:
    src_mark = nextToken();
    mustExpect(TokenType::SEMICOLON);
    return make_unique<ContinueStatement>(src_mark);
  case TokenType::BREAK:
    src_mark = nextToken();
    mustExpect(TokenType::SEMICOLON);
    return make_unique<BreakStatement>(src_mark);
  case TokenType::RETURN:
    src_mark = nextToken();
    if (peek().is_not(TokenType::SEMICOLON))
      expr = parseExpression();
    mustExpect(TokenType::SEMICOLON);
    return make_unique<ReturnStatement>(src_mark, std::move(expr));
  default:
    if (peek().is_not(TokenType::SEMICOLON)) {
      src_mark = peek();
      expr = parseExpression();
    }
    mustExpect(TokenType::SEMICOLON);
    return make_unique<ExpressionStatement>(src_mark, std::move(expr)); // FIXME
  }
}

std::unique_ptr<Statement> FastParser::parseLabeledStatement() {
  Token src_mark(peek());
  auto label = make_unique<IdentifierExpression>(nextToken());
  mustExpect(TokenType::COLON);
  auto stmt = parseStatement();
  return make_unique<LabeledStatement>(src_mark, std::move(label),
                                       std::move(stmt));
}

std::unique_ptr<Statement> FastParser::parseSelectionStatement() {
  Token src_mark(peek());
  mustExpect(TokenType::IF);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  auto ifPredicate = parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  auto ifBranch =
      peek().is(TokenType::IF) ? parseSelectionStatement() : parseStatement();
  if (mayExpect(TokenType::ELSE)) {
    auto elseBranch = parseStatement();
    return make_unique<IfElseStatement>(src_mark, std::move(ifPredicate),
                                        std::move(ifBranch),
                                        std::move(elseBranch));
  }

  return make_unique<IfElseStatement>(src_mark, std::move(ifPredicate),
                                      std::move(ifBranch));
}

std::unique_ptr<Statement> FastParser::parseIterationStatement() {
  Token src_mark(peek());
  mustExpect(TokenType::WHILE);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  auto whilePredicate = parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  auto whileBody = parseStatement();
  return make_unique<WhileStatement>(src_mark, std::move(whilePredicate),
                                     std::move(whileBody));
}

// Expressions
// (6.5.17) expression: assignment
std::unique_ptr<Expression> FastParser::parseExpression() {
  return parseAssignmentExpression();
}

// (6.5.16) assignment-expr: conditional-expr | unary-expr assignment-op
// assignment-expr (6.5.15) conditional-expr: logical-OR | logical-OR ?
// expression : conditional-expr
std::unique_ptr<Expression> FastParser::parseAssignmentExpression() {
  Token src_mark{TokenType::ASSIGN, getParserLocation()};
  auto lhs = parseUnaryExpression();               // LHS
  auto rhs = parseBinOpWithRHS(std::move(lhs), 1); // BinOpLeft + RHS
  return make_unique<AssignmentExpression>(src_mark, std::move(lhs),
                                           std::move(rhs));
}

std::unique_ptr<Expression>
FastParser::parseBinOpWithRHS(std::unique_ptr<Expression> lhs,
                              Precedence minPrec) {
  std::unique_ptr<Expression> ternary_middle;
  auto nextTokenPrec = peek().getPrecedence();

  while (true) {
    // If precedence of BinOp encounted is smaller than current Precedence
    // level return LHS.
    if (nextTokenPrec < minPrec)
      return std::move(lhs); // LHS

    auto binOpLeft = nextToken();

    // Handle conditional operator middle expression
    if (binOpLeft.getType() == TokenType::CONDITIONAL) {
      ternary_middle = parseExpression(); // Ternary middle
      mustExpect(TokenType::COLON);
    }

    auto rhs = parseUnaryExpression(); // RHS

    auto binOpLeftPrec = nextTokenPrec;
    nextTokenPrec = peek().getPrecedence(); // binOpRight

    if (binOpLeftPrec < nextTokenPrec) {
      // Evaluate RHS + binOpRight...
      rhs = parseBinOpWithRHS(std::move(rhs),
                              binOpLeftPrec); // new RHS after evaluation
    }

    // Make AST LHS = LHS + RHS or lhs + ternary_middle + rhs
    if (ternary_middle) {
      lhs = make_unique<ConditionalExpression>(
          binOpLeft, std::move(lhs), std::move(ternary_middle), std::move(rhs));
    } else {
      lhs = make_unique<BinaryExpression>(binOpLeft, std::move(lhs),
                                          std::move(rhs));
    }
    nextTokenPrec = peek().getPrecedence();
  }
}

// (6.5.3) unary-expression : postfix-expression
//                            unary-operator unary-expression
//                            sizeof unary-expression
//                            sizeof unary-expression
std::unique_ptr<Expression> FastParser::parseUnaryExpression() {
  Token op;
  std::unique_ptr<Expression> unary;

  if (peek().is(UNARY_OP)) {
    op = nextToken();
    unary = parseUnaryExpression();
    return make_unique<UnaryExpression>(op, std::move(unary));
  }

  if (peek().is(TokenType::SIZEOF)) {
    op = nextToken();
    if (mayExpect(TokenType::PARENTHESIS_OPEN)) {
      // Parse type
      // FIXME Parsing type-expression
      unary = make_unique<IdentifierExpression>(nextToken());
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      return make_unique<UnaryExpression>(op, std::move(unary));
    }

    unary = parseUnaryExpression();
    return make_unique<UnaryExpression>(op, std::move(unary));
  }
  return parsePostfixExpression();
}

// (6.5.2) postfix-expression : primary-expr
//                              postfix-expr [ expression ]
//                              postfix-expr ( argument-expr-list(opt) )
//                              postfix-expr . identifier
//                              postfix-expr -> identifier
std::unique_ptr<Expression> FastParser::parsePostfixExpression() {
  std::unique_ptr<Expression> post;
  Token src_mark(TokenType::GHOST, getParserLocation());
  Token op;
  auto primary = parsePrimaryExpression();
  switch (peek().getType()) {
  case TokenType::BRACKET_OPEN:
    op = nextToken();
    post = parseExpression();
    mustExpect(TokenType::BRACKET_CLOSE);
    return make_unique<PostfixExpression>(op, std::move(primary),
                                          std::move(post));
  case TokenType::PARENTHESIS_OPEN:
    op = nextToken();
    post = parseArgumentExpressionList();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return make_unique<PostfixExpression>(op, std::move(primary),
                                          std::move(post));
  case TokenType::DOT:
  case TokenType::ARROW:
    op = nextToken();
    if (peek().is(TokenType::IDENTIFIER)) {
      return make_unique<PostfixExpression>(
          op, std::move(primary),
          make_unique<IdentifierExpression>(nextToken()));
    }
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", expecting ->.");
    return std::unique_ptr<Expression>();
  default:
    return make_unique<PostfixExpression>(src_mark, std::move(primary));
    break;
  }
}

// (6.5.1) primary: identifer | constant | string-literal | ( expression )
std::unique_ptr<Expression> FastParser::parsePrimaryExpression() {
  std::unique_ptr<Expression> ret;
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    return make_unique<IdentifierExpression>(nextToken());
  case TokenType::NUMBER:
    return make_unique<ConstantExpression>(nextToken());
  case TokenType::CHARACTER:
    return make_unique<StringLiteralExpression>(
        nextToken()); // XXX CharacterExpression AST necessary?
  case TokenType::STRING:
    return make_unique<StringLiteralExpression>(nextToken());
  case TokenType::PARENTHESIS_OPEN:
    mustExpect(TokenType::PARENTHESIS_OPEN);
    ret = parseExpression();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return std::move(ret);
  default:
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                         "Unexpected Token: \"" + peek().name() +
                             "\", in primary expression.");
    return std::unique_ptr<PrimaryExpression>();
  }
}

std::unique_ptr<Expression> FastParser::parseArgumentExpressionList() {
  // TODO implement
  return make_unique<ArgumentExpressionList>();
}

} // namespace ccc
