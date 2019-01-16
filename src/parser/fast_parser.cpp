#include "fast_parser.hpp"

using namespace std;

namespace ccc {

struct EnumClassHash
{
  template <typename T>
  std::size_t operator()(T t) const
  {
    return static_cast<std::size_t>(t);
  }
};

static std::unordered_map<TokenType, UnaryOpValue, EnumClassHash> TokenToUnaryOpValue{
    {TokenType::AMPERSAND, UnaryOpValue::ADDRESS_OF},
    {TokenType::DEREFERENCE, UnaryOpValue::STAR},
    {TokenType::MINUS, UnaryOpValue::MINUS},
    {TokenType::NOT, UnaryOpValue::NOT}};

static std::unordered_map<TokenType, BinaryOpValue, EnumClassHash> TokenToBinaryOpValue{
    {TokenType::STAR, BinaryOpValue::MULTIPLY},
    {TokenType::PLUS, BinaryOpValue::ADD},
    {TokenType::MINUS, BinaryOpValue::SUBTRACT},
    {TokenType::LESS, BinaryOpValue::LESS_THAN},
    {TokenType::EQUAL, BinaryOpValue::EQUAL},
    {TokenType::NOT_EQUAL, BinaryOpValue::NOT_EQUAL},
    {TokenType::AND, BinaryOpValue::LOGICAL_AND},
    {TokenType::OR, BinaryOpValue::LOGICAL_OR},
    {TokenType::ASSIGN, BinaryOpValue::ASSIGN}};

// (6.9) translationUnit :: external-declaration+
unique_ptr<TranslationUnit> FastParser::parseTranslationUnit() {
  ExternalDeclarationListType external_decls;
  Token src_mark (peek());
  while (!fail() && peek().is_not(TokenType::ENDOFFILE)) {
    auto external_decl = parseExternalDeclaration();
    external_decls.push_back(move(external_decl));
  }
  return make_unique<TranslationUnit>(src_mark, move(external_decls));
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto
// declaration (see below), hence we parse upto that, then find out which type
// of AST node to be created.
unique_ptr<ExternalDeclaration> FastParser::parseExternalDeclaration() {
  return parseFuncDefOrDeclaration();
}

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement 
// (6.7)  declaration :: type-specifier declarator(opt) ;
unique_ptr<ExternalDeclaration> FastParser::parseFuncDefOrDeclaration(bool parseOnlyDecl) {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  unique_ptr<Declarator> identifier_node;
  Token src_mark = peek();
  bool structDefined; // XXX Better way
  auto type_node = parseTypeSpecifier(structDefined);

  if (fail()) {
    return unique_ptr<ExternalDeclaration>();
  }

  if (peek().is_not(TokenType::SEMICOLON)) {
    identifier_node = parseDeclarator();

    if (fail()) {
      return unique_ptr<ExternalDeclaration>();
    }
  }

  if (peek().is(TokenType::SEMICOLON)) {
    consume(TokenType::SEMICOLON);
    if (structDefined) {
      return make_unique<StructDeclaration>(src_mark, move(type_node), move(identifier_node));
    }
    if (isIdentiferFuncType) {
      isIdentiferFuncType = false;
      return make_unique<FunctionDeclaration>(src_mark, move(type_node), move(identifier_node));
    }
    return make_unique<DataDeclaration>(src_mark, move(type_node), move(identifier_node));
  }

  if (parseOnlyDecl) {
    parser_error(peek(), " Semicolon ");
    return unique_ptr<Declaration>();
  }

  // Function definition
  // XXX Ignore declaration-list for now
  if (peek().is(TokenType::BRACE_OPEN)) {
    auto fn_body = parseCompoundStatement();
    if (fail()) {
      return unique_ptr<ExternalDeclaration>();
    }
    return make_unique<FunctionDefinition>(src_mark, move(type_node), move(identifier_node), move(fn_body)); 
  }

  parser_error(peek(), "Function definition or declaration");
  return unique_ptr<ExternalDeclaration>();
}

unique_ptr<Type> FastParser::parseTypeSpecifier(bool & structDefined) {
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
      return unique_ptr<ScalarType>();
  }
}

// (6.7.2.1) struct-or-union-specifier :: struct identifier
// (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) {
// struct-declaration+ }
unique_ptr<StructType> FastParser::parseStructType(bool & structDefined) {
  Token src_mark (peek());
  string struct_name;
  ExternalDeclarationListType member_list;
  structDefined = false;

  consume(TokenType::STRUCT); // STRUCT keyword
  if (peek().is(TokenType::IDENTIFIER)) {
    struct_name = move(nextToken().getExtra());
  }

  if (peek().is(TokenType::BRACE_OPEN)) {
    structDefined = true;
    consume(TokenType::BRACE_OPEN);
    do {
      auto member = parseFuncDefOrDeclaration(true);
      member_list.push_back(move(member));
    } while (!fail() && !mayExpect(TokenType::BRACE_CLOSE));

    return make_unique<StructType>(src_mark, move(struct_name),
        move(member_list));
  }

  if (!struct_name.empty()) {
    return make_unique<StructType>(src_mark, move(struct_name));
  }

  parser_error(peek(), "struct identifier or struct-brace-open");
  return unique_ptr<StructType>();
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list ) 
// (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | ( parameter-list(opt) )+
unique_ptr<Declarator> FastParser::parseDeclarator(bool within_paren) {
  global_mark = peek();
  int ptrCount = 0; 
  if (peek().is(TokenType::STAR)) {
    // Parse Pointer (*) symbols
    parseList([&]() { ++ptrCount; },
              TokenType::STAR);
  }

  // In the case of function type, if pointer is within parenthesis then it is grouped with
  // function id, otherwise it becomes part of return type parameter. (Outermost pointer belongs to
  // return type and that is handled below.
  auto identifier = parseDirectDeclarator(within_paren, ptrCount);
  if (fail()) {
    return unique_ptr<Declarator>();
  } else {
    return std::move(identifier);
  }

  // TODO Abstract Declarator

  parser_error(peek(), " declarator (identifer or pointer symbol or \"(\") "); 
  return unique_ptr<Declarator>();
}

// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list ) 
unique_ptr<Declarator> FastParser::parseDirectDeclarator(bool in_paren, int ptrCount) {
  std::unique_ptr<Declarator> identifier;
  Token src_mark (peek());
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    consume(TokenType::PARENTHESIS_OPEN); // consume '('
    identifier = parseDeclarator(true);
    if (fail()) {
      return move(identifier);
    }
    mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
  } else if (peek().is(TokenType::IDENTIFIER)) {
    identifier = make_unique<DirectDeclarator>(src_mark,
                                          make_unique<VariableName>(nextToken(), src_mark.getExtra()));
  } else {
    parser_error(peek(), "identifier or parenthesized declarator");
    return unique_ptr<DirectDeclarator>();
  }

  ParamDeclarationListType param_list;
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    consume(TokenType::PARENTHESIS_OPEN);
    if (peek().is(C_TYPES)) {
      param_list = parseParameterList();
    }
    mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
    isIdentiferFuncType = true;
    if (ptrCount != 0 && in_paren) {
      identifier = make_unique<PointerDeclarator>(global_mark, std::move(identifier), ptrCount);
    }
    return make_unique<FunctionDeclarator>(src_mark, move(identifier), move(param_list));
  }

  if (ptrCount != 0) {
    identifier = make_unique<PointerDeclarator>(global_mark, std::move(identifier), ptrCount);
  }

  return std::move(identifier);
}
// NOTE:: Function return type is abstract declarator?

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
ParamDeclarationListType FastParser::parseParameterList() {
  ParamDeclarationListType param_list;
  do {
    auto param = parseParameterDeclaration();
    if (fail()) {
      return move(param_list);
    }
    param_list.push_back(move(param));
  } while (mayExpect(TokenType::COMMA));
  return move(param_list);
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
unique_ptr<ParamDeclaration> FastParser::parseParameterDeclaration() {
  Token src_mark (peek());
  bool structDefined = false;
  auto param_type = parseTypeSpecifier(structDefined);
  if (peek().is(TokenType::COMMA) || peek().is(TokenType::PARENTHESIS_CLOSE))
    return make_unique<ParamDeclaration>(src_mark, move(param_type), std::unique_ptr<Declarator>());    // Abstract-declaration
  auto param_name = parseDeclarator();
  if (fail()) {
    return unique_ptr<ParamDeclaration>();
  }
  return make_unique<ParamDeclaration>(src_mark, move(param_type), move(param_name));
}

unique_ptr<Statement> FastParser::parseCompoundStatement() {
  mustExpect(TokenType::BRACE_OPEN);
  /*
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
  */
  mustExpect(TokenType::BRACE_CLOSE);
  return unique_ptr<CompoundStmt>();
}

unique_ptr<Statement> FastParser::parseStatement() {
  return unique_ptr<Statement>();
}
/*
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
*/

// Expressions
// (6.5.17) expression: assignment
unique_ptr<Expression> FastParser::parseExpression() {
  return unique_ptr<Expression>();
}

/*
// (6.5.16) assignment-expr: conditional-expr | unary-expr assignment-op
// assignment-expr (6.5.15) conditional-expr: logical-OR | logical-OR ?
// expression : conditional-expr
void FastParser::parseAssignmentExpression() {
  parseUnaryExpression();               // LHS
  parseBinOpWithRHS(move(lhs), 1); // BinOpLeft + RHS
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
*/

// (6.5.3) unary-expression : postfix-expression
//                            unary-operator unary-expression
//                            sizeof ( type-name ) 
//                            sizeof unary-expression
std::unique_ptr<Expression> FastParser::parseUnaryExpression() {
  Token src_mark (peek()), op;
  if (src_mark.is(UNARY_OP)) {
    auto op = TokenToUnaryOpValue[nextToken().getType()];
    auto unary = parseUnaryExpression();
    if (fail()) {
      return std::unique_ptr<Expression>();
    }
    return make_unique<Unary>(src_mark, op, std::move(unary));
  }

  if (peek().is(TokenType::SIZEOF)) {
    consume(TokenType::SIZEOF);
    if (mayExpect(TokenType::PARENTHESIS_OPEN)) {
      auto type_name = parseTypeSpecifier();
      mustExpect(TokenType::PARENTHESIS_CLOSE);
      if (fail()) {
        return std::unique_ptr<Expression>();
      }
      return make_unique<SizeOf>(src_mark, std::move(type_name));
    }

    auto unary = parseUnaryExpression();
    if (fail()) {
      return std::unique_ptr<Expression>();
    }
    return make_unique<SizeOf>(src_mark, std::move(unary));
  }
  return parsePostfixExpression();
}

// (6.5.2) postfix-expression : primary-expr
//                              postfix-expr [ expression ]
//                              postfix-expr ( argument-expr-list(opt) )
//                              postfix-expr . identifier
//                              postfix-expr -> identifier
std::unique_ptr<Expression> FastParser::parsePostfixExpression() {
  Token src_mark (peek()), op;
  auto postfix = parsePrimaryExpression();
  if (fail()) {
    return std::unique_ptr<Expression>();
  }

  while (true) {
    switch (src_mark.getType()) {
      case TokenType::BRACKET_OPEN:
        op = nextToken();
        index = parseExpression();
        mustExpect(TokenType::BRACKET_CLOSE);
        postfix = make_unique<ArraySubscriptOp>(src_mark, std::move(postfix), std::move(index));
        break;
      case TokenType::PARENTHESIS_OPEN:
        op = nextToken();
        auto arg_list = parseArgumentExpressionList();
        mustExpect(TokenType::PARENTHESIS_CLOSE);
        postfix = make_unique<FunctionCall>(src_mark, std::move(postfix), std::move(arg_list));
        break;
      case TokenType::DOT:
      case TokenType::ARROW:
        auto op = (nextToken().getType() == TokenType::DOT) ? PostFixOpValue::DOT : PostFixOpValue::ARROW; 
        if (peek().is(TokenType::IDENTIFIER)) {
          auto m_name = peek().getExtra();
          auto identifer = make_unique<Identifier>(nextToken(), std::move(m_name));
          postfix = make_unique<MemberAccessOp>(src_mark, std::move(postfix), std::move(identifer));
          break;
        }
        parser_error(peek(), "identifier after member access operator (. or ->)");
        return std::unique_ptr<Expression>();
      default:
        return std::move(postfix);
    }
  }
}

// (6.5.1) primary: identifer | constant | string-literal | ( expression )
std::unique_ptr<Expression> FastParser::parsePrimaryExpression() {
  Token src_mark (peek());
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    return make_unique<VariableName>(nextToken(), src_mark.getExtra());
  case TokenType::NUMBER:
    return make_unique<Number>(nextToken(), stoi(src_mark.getExtra()));;
  case TokenType::CHARACTER:
    return make_unique<Character>(nextToken(), src_mark.getExtra());
  case TokenType::STRING:
    return make_unique<String>(nextToken(), src_mark.getExtra());
  case TokenType::PARENTHESIS_OPEN:
    mustExpect(TokenType::PARENTHESIS_OPEN);
    auto paren_expr = parseExpression();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
    return std::move(paren_expr);
  default:
    parser_error(peek(), "Expression or (");
    return std::unique_ptr<Expression>();
  }
}

ArgumentExpressionList FastParser::parseArgumentExpressionList() {
  // TODO implement
  ArgumentExpressionList empty_list;
  return empty_list;
}

} // namespace ccc
