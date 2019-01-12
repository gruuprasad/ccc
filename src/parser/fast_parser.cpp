#include "fast_parser.hpp"

namespace ccc {

static std::unordered_map<TokenType, TypeSpec> type_map { 
  {TokenType::VOID, TypeSpec::VOID},
  {TokenType::CHAR, TypeSpec::CHAR},
  {TokenType::INT, TypeSpec::INT},
  {TokenType::STRUCT, TypeSpec::STRUCT}
};

// (6.9) translationUnit :: external-declaration+
std::unique_ptr<ASTNode> FastParser::parseTranslationUnit() {
  std::vector<std::unique_ptr<Statement>> stmt_list;
  while (peek().is_not(TokenType::ENDOFFILE)) {
    auto stmt = parseExternalDeclaration();
    if (fail()) {
      // nullptr means, encounted error while parsing, hence we stop.
      // Invariant for error-free parsing - AST object pointer is non null.
      // XXX This doesn't hold for all classes, but that is deeply nested AST classes..
      // check this statement again. 
      return std::unique_ptr<TranslationUnit>();
    }
    stmt_list.emplace_back(std::move(stmt));
  }
  return make_unique<TranslationUnit>(std::move(stmt_list));
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto
// declaration (see below), hence we parse upto that, then find out which type
// of AST node to be created.
std::unique_ptr<Statement> FastParser::parseExternalDeclaration() { return parseFuncDefOrDeclaration(); }

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement (6.7)  declaration :: type-specifier declarator(opt) ;
// (6.7.2) type-specifier :: void | char | short | int | struct-or-union-specifier
std::unique_ptr<Statement> FastParser::parseFuncDefOrDeclaration() {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  Token src_mark;
  bool is_scalar_type;
  TypeSpec scalar_type;
  std::unique_ptr<TypeDeclaration> struct_type;

  if (peek().is(SCALAR_TYPES)) {
    is_scalar_type = true;
    src_mark = peek();
    scalar_type = type_map[nextToken().getType()];
  } else if (peek().is(TokenType::STRUCT)) {
    src_mark = peek();
    struct_type = parseStructTypeDeclaration();
  } else {
    parser_error(peek());
    return std::unique_ptr<Statement>();
  }

  std::unique_ptr<CompoundStatement> struct_body;
  if (peek().is(TokenType::BRACE_OPEN)) {
    struct_body = parseStructDefinition();
  }

  if (mayExpect(TokenType::SEMICOLON)) {
    if (is_scalar_type)
      return make_unique<DeclarationStatement>(src_mark,
                                make_unique<TypeDeclaration>(scalar_type));
    return make_unique<StructStatement>(src_mark, std::move(struct_type), std::move(struct_body));
  }

  // variable name in case of scalar type
  // or alias in case of struct type
  auto var_name = parseDeclarator();
  if (fail()) {
    return std::unique_ptr<Statement>();
  }

  if (mayExpect(TokenType::SEMICOLON)) {
    if (is_scalar_type) {
      return make_unique<DeclarationStatement>(src_mark,
          make_unique<TypeDeclaration>(scalar_type, std::move(var_name)));
    }
    return make_unique<StructStatement>(src_mark,
        std::move(struct_type), std::move(struct_body), std::move(var_name));
  }

  // Function definition
  while (peek().is(C_TYPES)) {
    parseDeclaration();
  }
  auto function_body = parseCompoundStatement();
  return std::unique_ptr<DeclarationStatement>(); // XXX Why not have functionStatement like struct
}

// (6.7.2.1) struct-or-union-specifier :: struct identifier
std::unique_ptr<TypeDeclaration> FastParser::parseStructTypeDeclaration() {
  auto type_decl = make_unique<TypeDeclaration>(type_map[nextToken().getType()]);
  if (peek().is(TokenType::IDENTIFIER)) {
    return make_unique<StructTypeDeclaration>(make_unique<IdentifierExpression>(nextToken()),
        std::move(type_decl));
  }
  parser_error(peek());
 return std::unique_ptr<StructTypeDeclaration>();
}

/// struct body parsing
// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) {
// struct-declaration+ } 
std::unique_ptr<CompoundStatement> FastParser::parseStructDefinition() {
  mustExpect(TokenType::BRACE_OPEN);
  do {
      parseStructMemberDeclaration(); // FIXME
    } while (peek().is_not(TokenType::BRACE_CLOSE));
 
  if (mustExpect(TokenType::BRACE_CLOSE)) {
    return std::unique_ptr<CompoundStatement>();
  }
  
  parser_error(peek());
  return std::unique_ptr<CompoundStatement>();
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator
// ( parameter-list ) (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | ( parameter-list(opt) )+
std::unique_ptr<Expression> FastParser::parseDeclarator() {
  int ptrCount = 0;        // > 0 means declarator is ptr type
  bool existIdent = false; // decides whether declarator is abstract or not.
  Token var_name;
  if (peek().is(TokenType::STAR)) {
    parseList([&]() { ++ptrCount; },
        TokenType::STAR); // consume  0 or more pointers
  }
  if (peek().is(TokenType::IDENTIFIER)) {
    var_name = nextToken();
    existIdent = true;
  }

  // Function type declarator magic happens here
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    if (peek().is(C_TYPES)) {
      mustExpect(TokenType::PARENTHESIS_OPEN);
      parseParameterList();
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      return std::unique_ptr<IdentifierExpression>(); // FIXME based on existIdent value create AST.
    }
    mustExpect(TokenType::PARENTHESIS_OPEN);
    parseDeclarator();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return std::unique_ptr<IdentifierExpression>(); // FIXME based on existIdent value create AST.
  }

  if (existIdent) {
    if (ptrCount != 0) {
      // pointer-type
      return make_unique<PointerTypeDeclaration>(ptrCount,
                          make_unique<IdentifierExpression>(var_name));
    }

    // non-pointer declarator type.
    return make_unique<IdentifierExpression>(var_name);
  }

  if (ptrCount != 0) {
    // abstract-pointer type.
    return make_unique<PointerTypeDeclaration>(ptrCount);
  }
  // TODO Abstract parameter-list

  parser_error(peek());
  return std::unique_ptr<IdentifierExpression>();
}

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
void FastParser::parseParameterList() {
  parseList([&]() { parseParameterDeclaration(); }, TokenType::COMMA);
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
void FastParser::parseParameterDeclaration() {
  // FIXME parseTypeSpecifier();
  if (peek().is(TokenType::COMMA))
    return;
  if (peek().is(TokenType::PARENTHESIS_CLOSE))
    return;
  parseDeclarator();
}

// (6.7)  declaration :: type-specifier declarator(opt) ;
std::unique_ptr<Statement> FastParser::parseDeclaration() {
  // FIXME auto type = parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    auto identifer = parseDeclarator();
  mustExpect(TokenType::SEMICOLON);
  return std::unique_ptr<DeclarationStatement>();
}

// (6.7.2.1) struct-declaration :: type-specifier declarator (opt) ;
void FastParser::parseStructMemberDeclaration() {
  // FIXME parseTypeSpecifier();
  if (peek().is_not(TokenType::SEMICOLON))
    parseList([&]() { parseDeclarator(); }, TokenType::COMMA);
  mustExpect(TokenType::SEMICOLON);
}

std::unique_ptr<Statement> FastParser::parseCompoundStatement() {
  Token src_mark (peek());
  std::vector<std::unique_ptr<Statement>> stmt_list;
  mustExpect(TokenType::BRACE_OPEN);
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is(C_TYPES)) {
      // FIXME auto type = parseTypeSpecifier();
      auto var = parseDeclarator();
      mustExpect(TokenType::SEMICOLON);
      stmt_list.emplace_back(std::unique_ptr<DeclarationStatement>());
    } else {
      auto stmt = parseStatement();
      stmt_list.emplace_back(std::move(stmt));
    }
  }
  mustExpect(TokenType::BRACE_CLOSE);
  return make_unique<CompoundStatement>(src_mark, std::move(stmt_list));
}

std::unique_ptr<Statement> FastParser::parseStatement() {
  Token src_mark;
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Statement> stmt;
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
      return make_unique<GotoStatement>(make_unique<IdentifierExpression>(nextToken()));
    }
    parser_error(peek());
    return std::unique_ptr<GotoStatement>();
  case TokenType::CONTINUE:
    stmt = make_unique<ContinueStatement>(nextToken());
    break;
  case TokenType::BREAK:
    stmt = make_unique<BreakStatement>(nextToken());
    break;
  case TokenType::RETURN:
    src_mark = nextToken();
    if (peek().is_not(TokenType::SEMICOLON)) {
      expr = parseExpression();
      if (!expr) break;
    }
    stmt = make_unique<ReturnStatement>(src_mark, std::move(expr));
    break;
  default:
    src_mark = peek();
    if (peek().is_not(TokenType::SEMICOLON)) {
      expr = parseExpression();
      if (!expr) break;
    }
    stmt = make_unique<ExpressionStatement>(src_mark, std::move(expr)); // FIXME
    break;
  }
  mustExpect(TokenType::SEMICOLON);
  return std::move(stmt);
}

std::unique_ptr<Statement> FastParser::parseLabeledStatement() {
  Token src_mark(peek());
  std::unique_ptr<LabeledStatement> stmt;
  auto label = make_unique<IdentifierExpression>(nextToken());
  if (mustExpect(TokenType::COLON)) {
    auto label_body = parseStatement();
    stmt = make_unique<LabeledStatement>(src_mark, std::move(label), std::move(label_body));
  }
  return std::move(stmt);
}

std::unique_ptr<Statement> FastParser::parseSelectionStatement() {
  Token src_mark(peek());
  std::unique_ptr<Statement> ifBranch;
  std::unique_ptr<Statement> elseBranch;
  mustExpect(TokenType::IF);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  auto ifPredicate = parseExpression();
  if (!fail() && ifPredicate) {
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    ifBranch = peek().is(TokenType::IF) ? parseSelectionStatement() : parseStatement();
    if (!fail() && mayExpect(TokenType::ELSE)) {
      elseBranch = parseStatement();
    }
  }
  return fail() ? std::unique_ptr<IfElseStatement>() :
    make_unique<IfElseStatement>(src_mark, std::move(ifPredicate), std::move(ifBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> FastParser::parseIterationStatement() {
  Token src_mark(peek());
  mustExpect(TokenType::WHILE);
  mustExpect(TokenType::PARENTHESIS_OPEN);
  auto whilePredicate = parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE);
  auto whileBody = parseStatement();
  return make_unique<WhileStatement>(src_mark, std::move(whilePredicate), std::move(whileBody));
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
    parser_error(peek());
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
    parser_error(peek());
    return std::unique_ptr<PrimaryExpression>();
  }
}

std::unique_ptr<Expression> FastParser::parseArgumentExpressionList() {
  // TODO implement
  return make_unique<ArgumentExpressionList>();
}

} // namespace ccc
