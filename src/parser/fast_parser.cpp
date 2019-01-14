#include "fast_parser.hpp"

namespace ccc {

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement 
// (6.7)  declaration :: type-specifier declarator(opt) ;
std::unique_ptr<ExternalDeclaration> FastParser::parseFuncDefOrDeclaration(bool parseOnlyDecl = false) {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  std::unique_ptr<Declarator> identifer_node;
  bool structDefined; // XXX Better way
  auto type_node = parseTypeSpecifier(structDefined);

  if (fail()) {
    return std::unique_ptr<ExternalDeclaration>();
  }

  if (peek().is_not(TokenType::SEMICOLON)) {
    identifier_node = parseDeclarator();

    if (fail()) {
      return std::unique_ptr<ExternalDeclaration>();
    }
  }

  if (peek().is(TokenType::SEMICOLON)) {
    consume(TokenType::SEMICOLON);
    if (structDefined) {
      return make_unique<StructDeclaration>(std::move(type_node), std::move(identifier_node));
    }
    if (identifier_node->getDeclaratorType() == DeclaratorType::Function) {
      return make_unique<FunctionDeclaration>(std::move(type_node), std::move(identifer_node));
    }

    return make_unique<DataDeclaration>(std::move(type_node), std::move(identifer_node));
  }

  if (parseOnlyDecl) {
    parser_error(peek(), " Semicolon ");
    return std::unique_ptr<Declaration>();
  }

  // Function definition
  // XXX Ignore declaration-list for now
  if (peek().is(TokenType::BRACE_OPEN)) {
    auto fn_body = parseCompoundStatement();
    if (fail()) {
      return std::unique_ptr<ExternalDeclaration>();
    }
    return make_unique<FunctionDefinition>(std::move(type_node), std::move(identifer_node), std::move(fn_body)); 
  }

  parser_error(peek(), "Function definition or declaration");
  return std::unique_ptr<ExternalDeclaration>();
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list ) 
// (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | ( parameter-list(opt) )+
std::unique_ptr<Declarator> FastParser::parseDeclarator(bool within_paren = false) {
  int ptrCount = 0;
  if (peek().is(TokenType::STAR)) {
    // Parse Pointer (*) symbols
    parseList([&]() { ++ptrCount; },
              TokenType::STAR);
  }

  // Parenthesized declarator
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    consume(TokenType::PARENTHESIS_OPEN); // consume '('
    auto identifer = parseDeclarator(true);
    if (fail()) {
      return std::move(identifer);
    }
    mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
    if (ptrCount != 0) {
      return make_unique<PointerDeclarator>(std;;move(identifer));
    }
    return std::move(identifer);
  }

  if (peek().is(TokenType::IDENTIFIER)) {
    auto var_expr = make_unique<VariableName>(nextToken());
    auto var_name = make_unique<DirectDeclarator>(std::move(var_expr));

    // Function type declarator magic happens here
    std::vector<std::unique_ptr<ParamDeclaration> param_list;
    if (peek().is(TokenType::PARENTHESIS_OPEN)) {
      consume(TokenType::PARENTHESIS_OPEN);
      if (peek().is(C_TYPES)) {
        param_list = parseParameterList();
      }
      mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
      if (ptrCount != 0) {
        if (within_paren) {
          auto fn_id = make_unique<PointerDeclarator>(std::move(var_name));
          return make_unique<FunctionDeclarator>(std::move(fn_id), std::move(param_list), false);
        }
      }
      return make_unique<FunctionDeclarator>(std::move(var_name), std::move(param_list));
    }

    if (ptrCount != 0) {
      return make_unique<PointerDeclarator>(std::move(var_name));
    }
    return std::move(var_name);
  }

  parser_error(peek(), " declarator (identifer or pointer symbol or ()"); 
  return std::unique_ptr<Declarator>();
}

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
std::vector<std::unique_ptr<ParamDeclaration> FastParser::parseParameterList() {
  std::vector<std::unique_ptr<ParamDeclaration> param_list;
  do {
    auto param = parseParameterDeclaration();
    if (fail()) {
      return std::move(param_list);
    }
    param_list.push_back(std::move(param));
  } while (mayExpect(TokenType::COMMA));
  return std::move(param_list);
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
std::unique_ptr<ParamDeclaration> FastParser::parseParameterDeclaration() {
  auto param_type = parseTypeSpecifier();
  if (peek().is(TokenType::COMMA) || peek().is(TokenType::PARENTHESIS_CLOSE))
    return make_unique<ParamDeclaration>(std::move(type));    // Abstract-declaration
  auto param_name = parseDeclarator();
  if (fail()) {
    return std::unique_ptr<ParamDeclaration>();
  }
  return make_unique<ParamDeclaration>(std::move(param_type), std::move(param_name));
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

// (6.9) translationUnit :: external-declaration+
void FastParser::parseTranslationUnit() {
  std::vector<std::unique_ptr<Extern>> extern_list;
  while (peek().is_not(TokenType::ENDOFFILE)) {
    auto extern_decl = parseExternalDeclaration();
    extern_list.push_back(std::move(extern_decl));
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
std::unique_ptr<Type> FastParser::parseTypeSpecifier(bool & structDefined) {
  switch(peek().getType()) {
    case TokenType::VOID:
      return make_unique<ScalarType>(nextToken(), ScalarTypeValue::VOID);
    case TokenType::CHAR:
      return make_unique<ScalarType>(nextToken(), ScalarTypeValue::CHAR);
    case TokenType::INT:
      return make_unique<ScalarType>(nextToken(), ScalarTypeValue::INT);
    case TokenType::STRUCT:
      return parseStructType(structDefined);
    default:
      parser_error(peek(), "Type-specifier");
      return std::unique_ptr<ScalarType>();
  }
}

// (6.7.2.1) struct-or-union-specifier :: struct identifier
// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) {
// struct-declaration+ }
std::unique_ptr<StructType> FastParser::parseStructType(bool & structDefined) {
  std::string struct_name;
  std::vector<std::unique_ptr<Declaration>> member_list;
  structDefined = false;

  consume(TokenType::STRUCT); // STRUCT keyword
  if (peek().is(TokenType::IDENTIFIER)) {
    struct_name = std::move(nextToken().getExtra());
  }

  if (peek().is(TokenType::BRACE_OPEN)) {
    structDefined = true;
    consume(TokenType::BRACE_OPEN);
    do {
      auto member = parseFuncDefOrDeclaration(true);
      member_list.push_back(std::move(member));
    } while (!fail() && !mayExpect(TokenType::BRACE_CLOSE));

    return make_unique<StructType>(std::move(struct_name),
        std::move(member_list));
  }

  parser_error(peek(), "struct identifier or struct-brace-open");
  return unique_ptr<StructType>();
}

} // namespace ccc
