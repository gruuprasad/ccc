#include "fast_parser.hpp"

using namespace std;

namespace ccc {

static std::unordered_map<TokenType, UnaryOpValue, EnumClassHash>
    TokenToUnaryOpValue{{TokenType::AMPERSAND, UnaryOpValue::ADDRESS_OF},
                        {TokenType::STAR, UnaryOpValue::DEREFERENCE},
                        {TokenType::MINUS, UnaryOpValue::MINUS},
                        {TokenType::NOT, UnaryOpValue::NOT}};

static std::unordered_map<TokenType, BinaryOpValue, EnumClassHash>
    TokenToBinaryOpValue{{TokenType::STAR, BinaryOpValue::MULTIPLY},
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
  Token src_mark(peek());
  if (src_mark.getType() == TokenType::ENDOFFILE) {
    parser_error(Token(TokenType::ENDOFFILE, 1, 0));
  }
  while (!fail() && peek().is_not(TokenType::ENDOFFILE)) {
    auto external_decl = parseExternalDeclaration();
    external_decls.push_back(move(external_decl));
  }
  return make_unique<TranslationUnit>(src_mark, std::move(external_decls));
}

// (6.9) external-declaration :: function-definition | declaration
// Both non-terminals (func-def and declaration) on rhs has same terms upto
// declaration (see below), hence we parse upto that, then find out which type
// of AST node to be created.
unique_ptr<ExternalDeclaration> FastParser::parseExternalDeclaration() {
  return parseFuncDefOrDeclaration();
}

// Method to handle only type declaration.
unique_ptr<ExternalDeclaration> FastParser::parseDeclaration() {
  unique_ptr<Declarator> identifier_node;
  Token src_mark(peek());
  auto type_node = parseTypeSpecifier();

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
    if (type_node.second) {
      return make_unique<StructDeclaration>(src_mark, move(type_node.first),
                                            move(identifier_node));
    }
    {
      return make_unique<DataDeclaration>(src_mark, move(type_node.first),
                                          move(identifier_node));
    }
  }

  parser_error(peek(), " Semicolon at the end of declaration.");
  return unique_ptr<ExternalDeclaration>();
}

// (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
// compound-statement
// (6.7)  declaration :: type-specifier declarator(opt) ;
unique_ptr<ExternalDeclaration> FastParser::parseFuncDefOrDeclaration() {
  // Presence or absence of SEMICOLON determines whether declaration or
  // function-definition
  abstract = false;
  unique_ptr<Declarator> identifier_node;
  Token src_mark = peek();
  auto type_node = parseTypeSpecifier();

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
    if (type_node.second || type_node.first->getStructType()) {
      return make_unique<StructDeclaration>(src_mark, move(type_node.first),
                                            move(identifier_node));
    }
    if (isFunctionIdentifer) {
      isFunctionIdentifer = false;
      return make_unique<FunctionDeclaration>(src_mark, move(type_node.first),
                                              move(identifier_node));
    }
    return make_unique<DataDeclaration>(src_mark, move(type_node.first),
                                        move(identifier_node));
  }
  isFunctionIdentifer = false;

  if (abstract) {
    parser_error(abstract_loc,
                 "identifier, parameter list or parenthesized declarator");
    return unique_ptr<ExternalDeclaration>();
  }

  // Function definition
  // XXX Ignore declaration-list for now
  if (peek().is(TokenType::BRACE_OPEN)) {
    auto fn_body = parseCompoundStatement();
    if (fail()) {
      return unique_ptr<ExternalDeclaration>();
    }
    return make_unique<FunctionDefinition>(
        src_mark, move(type_node.first), move(identifier_node), move(fn_body));
  }

  parser_error(peek(), "Function definition or declaration");
  return unique_ptr<ExternalDeclaration>();
}

pair<unique_ptr<Type>, bool> FastParser::parseTypeSpecifier() {
  switch (peek().getType()) {
  case TokenType::VOID:
    return make_pair(
        make_unique<ScalarType>(nextToken(), ScalarTypeValue::VOID), false);
  case TokenType::CHAR:
    return make_pair(
        make_unique<ScalarType>(nextToken(), ScalarTypeValue::CHAR), false);
  case TokenType::INT:
    return make_pair(make_unique<ScalarType>(nextToken(), ScalarTypeValue::INT),
                     false);
  case TokenType::STRUCT:
    return parseStructType();
  default:
    parser_error(peek(), "Type-specifier");
    return make_pair(unique_ptr<ScalarType>(), false);
  }
}

// (6.7.2.1) struct-or-union-specifier :: struct identifier
// (6.7.2.1) struct-or-union-specifier :: struct identifier(opt) {
// struct-declaration+ }
pair<unique_ptr<StructType>, bool> FastParser::parseStructType() {
  Token src_mark(peek());
  Token struct_name;
  ExternalDeclarationListType member_list = ExternalDeclarationListType();

  consume(TokenType::STRUCT); // STRUCT keyword
  if (peek().is(TokenType::IDENTIFIER)) {
    struct_name = nextToken();
  }

  if (peek().is(TokenType::BRACE_OPEN)) {
    consume(TokenType::BRACE_OPEN);
    while (!fail() && !mayExpect(TokenType::BRACE_CLOSE)) {
      auto member = parseDeclaration();
      member_list.push_back(move(member));
    }

    if (!struct_name.getExtra().empty()) {
      return make_pair(
          make_unique<StructType>(
              src_mark,
              make_unique<VariableName>(struct_name, struct_name.getExtra()),
              move(member_list)),
          true);
    } else {
      return make_pair(
          make_unique<StructType>(src_mark, nullptr, move(member_list)), true);
    }
  }

  if (!struct_name.getExtra().empty()) {
    return make_pair(make_unique<StructType>(
                         src_mark, make_unique<VariableName>(
                                       struct_name, struct_name.getExtra())),
                     false);
  }

  parser_error(peek(), "struct identifier or struct-brace-open");
  return make_pair(unique_ptr<StructType>(), false);
}

// Function to handle all kinds of declarator.
// (6.7.6)  declarator :: pointer(opt) direct-declarator
// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator
// ( parameter-list ) (6.7.6) abstract-declarator :: pointer | pointer(opt)
// direct-abstract-declarator (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | direct-abstract-declarator ( parameter-list(opt) )+
unique_ptr<Declarator> FastParser::parseDeclarator(bool within_paren) {
  global_mark = peek();
  int ptrCount = 0;
  if (peek().is(TokenType::STAR)) {
    // Parse Pointer (*) symbols
    do {
      consume(TokenType::STAR);
      ptrCount++;
    } while (peek().is(TokenType::STAR));
  }

  if (ptrCount != 0 && peek().is_not(TokenType::PARENTHESIS_OPEN) &&
      peek().is_not(TokenType::IDENTIFIER)) {
    // Abstract-declarator::pointer
    abstract = true;
    abstract_loc = peek();
    return make_unique<AbstractDeclarator>(global_mark, AbstractDeclType::Data,
                                           ptrCount);
  }

  auto identifier = parseDirectDeclarator(within_paren, ptrCount);
  if (fail()) {
    return unique_ptr<Declarator>();
  } else {
    return identifier;
  }

  parser_error(peek(), " declarator (identifier or pointer symbol or \"(\") ");
  return unique_ptr<Declarator>();
}

// (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator
// ( parameter-list )
// (6.7.6) direct-abstract-declarator :: (
// abstract-declarator ) | direct-abstract-declarator ( parameter-list(opt) )
unique_ptr<Declarator> FastParser::parseDirectDeclarator(bool, int ptrCount) {
  std::unique_ptr<Declarator> identifier;
  Token src_mark(peek());
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    consume(TokenType::PARENTHESIS_OPEN); // consume '('
    identifier = parseDeclarator(true);
    if (fail()) {
      return identifier;
    }
    mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
  } else if (peek().is(TokenType::IDENTIFIER)) {
    identifier = make_unique<DirectDeclarator>(
        src_mark, make_unique<VariableName>(nextToken(), src_mark.getExtra()));
  } else if (peek().is(C_TYPES)) {
    abstract_loc = peek();
    abstract = true;
    auto param_list = parseParameterList();
    isFunctionIdentifer = true;
    identifier = make_unique<AbstractDeclarator>(src_mark,
                                                 AbstractDeclType::Function, 0);
    auto return_ptr = make_unique<AbstractDeclarator>(
        global_mark, AbstractDeclType::Data, ptrCount);
    return make_unique<FunctionDeclarator>(src_mark, move(identifier),
                                           move(param_list), move(return_ptr));
  } else if (peek().is(TokenType::PARENTHESIS_CLOSE)) {
    abstract = true;
    abstract_loc = peek();
    return make_unique<AbstractDeclarator>(global_mark, AbstractDeclType::Data,
                                           ptrCount);
  } else {
    parser_error(peek(),
                 "identifier, parameter list or parenthesized declarator");
    return unique_ptr<DirectDeclarator>();
  }

  ParamDeclarationListType param_list = ParamDeclarationListType();
  if (peek().is(TokenType::PARENTHESIS_OPEN)) {
    consume(TokenType::PARENTHESIS_OPEN);
    if (peek().is(C_TYPES)) {
      param_list = parseParameterList();
    }
    mustExpect(TokenType::PARENTHESIS_CLOSE, " ) ");
    isFunctionIdentifer = true;

    //    if (ptrCount != 0 && in_paren) {
    //      identifier = make_unique<PointerDeclarator>(
    //          global_mark, std::move(identifier), ptrCount);
    //    }

    auto return_ptr = make_unique<AbstractDeclarator>(
        global_mark, AbstractDeclType::Data, ptrCount);

    return make_unique<FunctionDeclarator>(src_mark, move(identifier),
                                           move(param_list), move(return_ptr));
  }

  if (ptrCount != 0) {
    auto tmp = identifier->getFunctionDeclarator();
    if (tmp) {
      auto return_ptr = make_unique<AbstractDeclarator>(
          global_mark, AbstractDeclType::Data,
          tmp->return_ptr->getAbstractDeclarator()->pointerCount + ptrCount);
      return make_unique<FunctionDeclarator>(
          tmp->getTokenRef(), move(tmp->identifier), move(tmp->param_list),
          move(return_ptr));
    } else
      identifier = make_unique<PointerDeclarator>(
          global_mark, std::move(identifier), ptrCount);
  }
  return identifier;
}

// (6.7.6)  parameter-list :: parameter-declaration (comma-separated)
ParamDeclarationListType FastParser::parseParameterList() {
  auto tmp = abstract;
  abstract = false;
  ParamDeclarationListType param_list = ParamDeclarationListType();
  do {
    auto param = parseParameterDeclaration();
    if (fail()) {
      return param_list;
    }
    param_list.push_back(move(param));
  } while (mayExpect(TokenType::COMMA));
  abstract = tmp;
  return param_list;
}

// (6.7.5) parameter-declaration :: type-specifier declarator | type-specifier
// abstract-declarator(opt)
unique_ptr<ParamDeclaration> FastParser::parseParameterDeclaration() {
  Token src_mark(peek());
  auto param_type = parseTypeSpecifier().first;
  if (peek().is(TokenType::COMMA) || peek().is(TokenType::PARENTHESIS_CLOSE))
    return make_unique<ParamDeclaration>(
        src_mark, move(param_type),
        std::unique_ptr<Declarator>()); // Abstract-declaration
  auto param_name = parseDeclarator();
  if (fail()) {
    return unique_ptr<ParamDeclaration>();
  }
  return make_unique<ParamDeclaration>(src_mark, move(param_type),
                                       move(param_name));
}

unique_ptr<Statement> FastParser::parseCompoundStatement() {
  auto src_mark(peek());
  std::vector<std::unique_ptr<ASTNode>> stmts =
      std::vector<std::unique_ptr<ASTNode>>();
  std::unique_ptr<ASTNode> stmt;
  mustExpect(TokenType::BRACE_OPEN, " open brace ({) ");
  while (peek().is_not(TokenType::BRACE_CLOSE)) {
    if (peek().is(C_TYPES)) {
      stmt = parseDeclaration();
    } else {
      stmt = parseStatement();
    }
    if (fail()) {
      return std::unique_ptr<CompoundStmt>();
    }
    stmts.push_back(std::move(stmt));
  }
  mustExpect(TokenType::BRACE_CLOSE, " close brace (}) ");
  return make_unique<CompoundStmt>(src_mark, std::move(stmts));
}

unique_ptr<Statement> FastParser::parseStatement() {
  std::unique_ptr<Expression> expr_node;
  auto src_mark(peek());

  if (peek().getType() == TokenType::IDENTIFIER &&
      peek(1).getType() == TokenType::COLON) {
    return parseLabeledStatement();
  }
  switch (peek().getType()) {
  case TokenType::BRACE_OPEN:
    return parseCompoundStatement();
  case TokenType::IF:
    return parseSelectionStatement();
  case TokenType::WHILE:
    return parseIterationStatement();
  case TokenType::GOTO:
    consume(TokenType::GOTO);
    if (peek().is(TokenType::IDENTIFIER)) {
      auto name = peek().getExtra();
      std::unique_ptr<VariableName> identifier =
          make_unique<VariableName>(nextToken(), std::move(name));
      mustExpect(TokenType::SEMICOLON, " Semicolon (;) ");
      return make_unique<Goto>(src_mark, std::move(identifier));
    }
    parser_error(peek());
    return std::unique_ptr<Statement>();
  case TokenType::CONTINUE:
    consume(TokenType::CONTINUE);
    mustExpect(TokenType::SEMICOLON, " Semicolon (;) ");
    return make_unique<Continue>(src_mark);
  case TokenType::BREAK:
    consume(TokenType::BREAK);
    mustExpect(TokenType::SEMICOLON, " Semicolon (;) ");
    return make_unique<Break>(src_mark);
  case TokenType::RETURN:
    consume(TokenType::RETURN);
    if (peek().is_not(TokenType::SEMICOLON)) {
      expr_node = parseExpression();
    }
    mustExpect(TokenType::SEMICOLON, " Semicolon (;) ");
    return make_unique<Return>(src_mark, std::move(expr_node));
  default:
    if (peek().is_not(TokenType::SEMICOLON)) {
      expr_node = parseExpression();
    }
    mustExpect(TokenType::SEMICOLON, " Semicolon (;) ");
    return make_unique<ExpressionStmt>(src_mark, std::move(expr_node));
  }
  parser_error(peek(), "Statement, unrecognized statement type");
  return std::unique_ptr<Statement>();
}

std::unique_ptr<Statement> FastParser::parseLabeledStatement() {
  auto src_mark(peek());
  auto name = peek().getExtra();
  auto label_node = make_unique<VariableName>(nextToken(), std::move(name));
  if (mustExpect(TokenType::COLON)) {
    auto stmt_node = parseStatement();
    return make_unique<Label>(src_mark, std::move(label_node),
                              std::move(stmt_node));
  }
  return std::unique_ptr<Label>();
}

std::unique_ptr<Statement> FastParser::parseSelectionStatement() {
  Token src_mark(peek());
  mustExpect(TokenType::IF);
  mustExpect(TokenType::PARENTHESIS_OPEN, " parenthesis after if keyword");
  auto predicate = parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE,
             " parenthesis close after if condition ");
  auto ifBranch = parseStatement();
  std::unique_ptr<Statement> elseBranch;
  if (!fail() && mayExpect(TokenType::ELSE)) {
    elseBranch = parseStatement();
  }

  if (fail())
    return std::unique_ptr<IfElse>();

  return make_unique<IfElse>(src_mark, std::move(predicate),
                             std::move(ifBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> FastParser::parseIterationStatement() {
  Token src_mark(peek());
  mustExpect(TokenType::WHILE, " while keyword ");
  mustExpect(TokenType::PARENTHESIS_OPEN,
             " parenthesis open after while keyword ");
  auto predicate = parseExpression();
  mustExpect(TokenType::PARENTHESIS_CLOSE,
             " parenthesis close after while condition ");
  if (fail()) {
    return std::unique_ptr<While>();
  }
  auto block = parseStatement();
  return make_unique<While>(src_mark, std::move(predicate), std::move(block));
}

// Expressions
// (6.5.17) expression: assignment
unique_ptr<Expression> FastParser::parseExpression() {
  return parseAssignmentExpression();
}

// assignment-expression is top level grammar for different types of
// expression (like unary, binary, ternary and assignment (=) even though
// non-terminal name is bit misleading.
// (6.5.16) assignment-expr: conditional-expr | unary-expr assignment-op
// assignment-expr (6.5.15) conditional-expr: logical-OR | logical-OR ?
// expression : conditional-expr
std::unique_ptr<Expression> FastParser::parseAssignmentExpression() {

  auto lhs = parseUnaryExpression(); // LHS or first operand
  Token src_mark(peek());
  // Expression contains assignment op
  if (peek().is(TokenType::ASSIGN)) {
    consume(TokenType::ASSIGN);
    auto rhs = parseAssignmentExpression(); // Recursively parse assignment
    if (fail()) {
      return std::unique_ptr<Expression>();
    }
    return make_unique<Assignment>(src_mark, std::move(lhs), std::move(rhs));
  }

  // Expression is binary
  if (peek().is(BINARY_OP)) {
    return parseBinOpWithRHS(std::move(lhs), 14);
  }

  // Expression is unary
  return lhs;
}

std::unique_ptr<Expression>
FastParser::parseBinOpWithRHS(std::unique_ptr<Expression> lhs,
                              Precedence minPrec) {
  auto nextTokenPrec = peek().getPrecedence();

  while (true) {
    // If precedence of BinOp encounted is smaller than current Precedence
    // level return LHS.
    if (nextTokenPrec >= minPrec) {
      if (fail())
        return std::unique_ptr<Expression>();
      return lhs; // LHS
    }

    auto binOpLeft = nextToken();
    auto binOpLeftValue = TokenToBinaryOpValue[binOpLeft.getType()];

    // Handle conditional operator middle expression
    bool ternayOp = false;
    std::unique_ptr<Expression> ternary_middle;
    if (binOpLeft.getType() == TokenType::CONDITIONAL) {
      ternayOp = true;
      ternary_middle = parseExpression(); // Ternary middle
      mustExpect(TokenType::COLON, " colon in ternary operator ");
    }

    auto rhs = parseUnaryExpression(); // RHS

    auto binOpLeftPrec = nextTokenPrec;
    nextTokenPrec = peek().getPrecedence(); // binOpRight

    if (ternayOp) { // rhs of : with maximal precedence
      rhs = parseBinOpWithRHS(std::move(rhs), 15);
    } else if (binOpLeftPrec > nextTokenPrec) {
      // Evaluate RHS + binOpRight...
      rhs = parseBinOpWithRHS(std::move(rhs),
                              binOpLeftPrec); // new RHS after evaluation
    }

    // Make AST LHS = LHS + RHS or lhs + ternary_middle + rhs
    if (ternayOp) {
      lhs = make_unique<Ternary>(binOpLeft, std::move(lhs),
                                 std::move(ternary_middle), std::move(rhs));
    } else {
      if (binOpLeftValue == BinaryOpValue::ASSIGN)
        lhs =
            make_unique<Assignment>(binOpLeft, std::move(lhs), std::move(rhs));
      else
        lhs = make_unique<Binary>(binOpLeft, binOpLeftValue, std::move(lhs),
                                  std::move(rhs));
    }
    nextTokenPrec = peek().getPrecedence();
  }
}

// (6.5.3) unary-expression : postfix-expression
//                            unary-operator unary-expression
//                            sizeof ( type-name )
//                            sizeof unary-expression
std::unique_ptr<Expression> FastParser::parseUnaryExpression() {
  Token src_mark(peek()), op;
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
    if (peek().is(TokenType::PARENTHESIS_OPEN) && peek(1).is(C_TYPES)) {
      consume(TokenType::PARENTHESIS_OPEN);
      auto type_name = parseTypeSpecifier().first;
      int par = 0;
      int star = 0;
      bool loop = true;
      while (loop) {
        switch (peek().getType()) {
        case TokenType::STAR:
          consume(TokenType::STAR);
          star++;
          break;
        case TokenType::PARENTHESIS_OPEN:
          consume(TokenType::PARENTHESIS_OPEN);
          par++;
          break;
        case TokenType::PARENTHESIS_CLOSE:
          if (par > 0)
            consume(TokenType::PARENTHESIS_CLOSE);
          loop = par > 0;
          par--;
          break;
        default:
          loop = false;
          break;
        }
      }
      mustExpect(TokenType::PARENTHESIS_CLOSE, " parenthesis close ");
      if (fail()) {
        return std::unique_ptr<Expression>();
      }
      if (star > 0)
        type_name =
            make_unique<AbstractType>(src_mark, std::move(type_name), star);
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
  Token src_mark(peek()), op;
  PostFixOpValue postfixOp;
  ExpressionListType arg_list;
  std::string m_name;
  std::unique_ptr<Expression> post_operand;
  auto postfix = parsePrimaryExpression();
  if (fail()) {
    return std::unique_ptr<Expression>();
  }
  src_mark = peek();
  while (true) {
    switch (peek().getType()) {
    case TokenType::BRACKET_OPEN:
      consume(TokenType::BRACE_OPEN);
      post_operand = parseExpression();
      mustExpect(TokenType::BRACKET_CLOSE, " bracket close ");
      postfix = make_unique<ArraySubscriptOp>(src_mark, std::move(postfix),
                                              std::move(post_operand));
      break;
    case TokenType::PARENTHESIS_OPEN:
      arg_list = parseArgumentExpressionList();
      postfix = make_unique<FunctionCall>(src_mark, std::move(postfix),
                                          std::move(arg_list));
      break;
    case TokenType::DOT:
    case TokenType::ARROW:
      postfixOp = (nextToken().getType() == TokenType::DOT)
                      ? PostFixOpValue::DOT
                      : PostFixOpValue::ARROW;
      if (peek().is(TokenType::IDENTIFIER)) {
        m_name = peek().getExtra();
        post_operand =
            make_unique<VariableName>(nextToken(), std::move(m_name));
        postfix = make_unique<MemberAccessOp>(
            src_mark, postfixOp, std::move(postfix), std::move(post_operand));
        break;
      }
      parser_error(peek(), "identifier after member access operator (. or ->)");
      return std::unique_ptr<Expression>();
    default:
      return postfix;
    }
  }
}

// (6.5.1) primary: identifier | constant | string-literal | ( expression )
std::unique_ptr<Expression> FastParser::parsePrimaryExpression() {
  std::unique_ptr<Expression> paren_expr;
  std::string num_str;
  Token src_mark(peek());
  switch (peek().getType()) {
  case TokenType::IDENTIFIER:
    return make_unique<VariableName>(nextToken(), src_mark.getExtra());
  case TokenType::NUMBER:
    num_str = src_mark.getExtra();
    if (num_str.size() > 1 && src_mark.getExtra()[0] == '0') {
      parser_error(src_mark, "Bad number, cannot start with 0");
      return std::unique_ptr<Expression>();
    }
    if (num_str.size() >= std::numeric_limits<long>::digits10) {
      parser_error(src_mark, "Bad i32");
      return std::unique_ptr<Expression>();
    }
    return make_unique<Number>(nextToken(), stol(num_str));
  case TokenType::CHARACTER:
    return make_unique<Character>(nextToken(), src_mark.getExtra());
  case TokenType::STRING:
    return make_unique<String>(nextToken(), src_mark.getExtra());
  case TokenType::PARENTHESIS_OPEN:
    mustExpect(TokenType::PARENTHESIS_OPEN, " open-parenthesis ");
    paren_expr = parseExpression();
    mustExpect(TokenType::PARENTHESIS_CLOSE, " close parenthesis ");
    return paren_expr;
  default:
    parser_error(peek(), "Expression or (");
    return std::unique_ptr<Expression>();
  }
}

ExpressionListType FastParser::parseArgumentExpressionList() {
  ExpressionListType arg_list = ExpressionListType();
  mustExpect(TokenType::PARENTHESIS_OPEN);
  if (peek().is_not(TokenType::PARENTHESIS_CLOSE)) {
    while (true) {
      auto arg_i = parseAssignmentExpression();
      arg_list.push_back(std::move(arg_i));
      if (peek().is(TokenType::PARENTHESIS_CLOSE)) {
        consume(TokenType::PARENTHESIS_CLOSE);
        return arg_list;
      }
      mustExpect(TokenType::COMMA);
      if (fail()) {
        return arg_list;
      }
    }
  }

  consume(TokenType::PARENTHESIS_CLOSE);
  return arg_list;
}

} // namespace ccc
