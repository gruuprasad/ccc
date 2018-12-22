#ifndef C4_PARSER_HPP
#define C4_PARSER_HPP

#include "../lexer/token.hpp"
#include "../utils/assert.hpp"
#include "../utils/macros.hpp"
#include <vector>

namespace ccc {

enum PARSE_TYPE { TRANSLATIONUNIT, EXPRESSION, STATEMENT, DECLARATION };

class FastParser {
public:
  FastParser(std::vector<Token> &tokens_) : tokens(tokens_) {
    tokens.emplace_back(TokenType::TOKENEND, 0, 0);
  }

  void parse(PARSE_TYPE type = PARSE_TYPE::TRANSLATIONUNIT) {
    switch (type) {
    case PARSE_TYPE::TRANSLATIONUNIT:
      return parseTranslationUnit();
    case PARSE_TYPE::EXPRESSION:
      return parseExpression();
    case PARSE_TYPE::STATEMENT:
      return parseStatement();
    case PARSE_TYPE::DECLARATION:
      return parseDeclarations();
    default:
      error = "Unknown parse type";
    }
  }

  bool fail() const { return !error.empty(); }
  std::string getError() { return error; }

private:
  Token nextToken() { return tokens[curTokenIdx++]; }

  bool consume() {
    curTokenIdx++;
    return true;
  }

  bool expect(TokenType tok) {
    if (peek().is(tok)) {
      return consume();
    } else {
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                           "Unexpected Token: \"" + peek().name() +
                               "\", expecting  \"" + "\"");
      return false;
    }
    // FIXME get string version of token?
  }

  template <typename T, typename... Args> bool expect(T first, Args... args) {
    return expect(first) && expect(args...);
  }

  const Token &peek(int k = 0) const {
    // TODO bound check. Append k number of empty tokens at the end of tokens
    // so we don't need to bound check.
    return tokens[curTokenIdx + k];
  }

  template <typename F> void parseList(F word, TokenType delimit) {
    do {
      word();
    } while (peek().is(delimit) && consume());
  }

  // Grammar Rules we plan to implement.
  // (6.9) translationUnit :: external-declaration+
  // (6.9) external-declaration :: function-definition | declaration
  // (6.9.1) function-definition :: type-specifier declarator declaration+(opt)
  // compound-statement (6.7)  declaration :: type-specifier declarator(opt) ;
  // (6.7.2) type-specifier :: void | char | short | int |
  // struct-or-union-specifier (6.7.2.1) struct-or-union-specifier :: struct
  // identifer(opt) { struct-declaration+ } | struct-or-union identifier
  // (6.7.2.1) struct-declaration :: type-specifier declarator (opt) ;
  // (6.7.6)  declarator :: pointer(opt) direct-declarator
  // (6.7.6)  direct-declarator :: identifier | ( declarator ) |
  // direct-declarator ( parameter-list ) (6.7.6)  parameter-list ::
  // (type-specifier declarator)+ (comma-separated)
  // TODO Add rules -> parameter-declaration :: type-specifiers
  // abstract-declarator(opt)
  //                   abstract-declarator :: pointer | pointer(opt)
  //                   direct-abstract-declarator direct-abstract-declarator ::
  //                   ( abstract-declarator) | ( parameter-type-list(opt) )+

  void parseTranslationUnit();
  void parseExternalDeclaration();
  void parseFuncDefOrDeclaration();

  // declarations
  void parseDeclarations();
  void parseTypeSpecifiers();
  void parseDeclarator();
  void parseDirectDeclarator();
  void parseParameterList();
  void parseStructOrUnionSpecifier();
  void parseStructDeclaration();

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
  // relational (6.5.8) relational: shift | relational < shift | relational >
  // shift | relational <= shift | relational >= shift (6.5.7) shift: additive |
  // shift << additive | shift >> additive (6.5.6) additive: multiplicative |
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

  void parseExpression();
  void parsePrimary();
  void parsePrimaryExpression();
  void parseUnaryExpression();
  void parseArgumentExpressionList();
  void parsePostfixExpression();

  // Statements
  void parseCompoundStatement();
  void parseBlockItemList();
  void parseStatement();
  void parseLabeledStatement();
  void parseSelectionStatement();
  void parseIterationStatement();

  std::vector<Token> &tokens;
  std::size_t curTokenIdx = 0;
  std::string error = "";
};

} // namespace ccc
#endif
