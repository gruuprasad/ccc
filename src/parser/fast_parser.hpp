#include <utility>

#ifndef C4_PARSER_HPP
#define C4_PARSER_HPP

#include "../ast/ast_node.hpp"
#include "../lexer/token.hpp"
#include "../utils/assert.hpp"
#include "../utils/macros.hpp"
#include <execinfo.h>
#include <vector>

namespace ccc {

enum PARSE_TYPE { TRANSLATIONUNIT, EXPRESSION, STATEMENT, DECLARATION };

class FastParser {
public:
  FastParser(std::vector<Token> &tokens_) : tokens(tokens_) {
    tokens.emplace_back(TokenType::TOKENEND, 0, 0);
  }

  ASTNode *parse(PARSE_TYPE type = PARSE_TYPE::TRANSLATIONUNIT,
                 bool debug = false) {
#if DEBUG
    this->debug = debug;
#endif
    switch (type) {
    case PARSE_TYPE::TRANSLATIONUNIT:
      return parseTranslationUnit();
    case PARSE_TYPE::EXPRESSION:
      return parseExpression();
    case PARSE_TYPE::STATEMENT:
      return parseStatement();
    case PARSE_TYPE::DECLARATION:
      parseDeclarations();
      return nullptr;
    default:
      error = "Unknown parse type";
      return nullptr;
    }
  }

  bool fail() const { return !error.empty(); }
  std::string getError() { return error; }

private:
  bool debug;

  Token nextToken() { return tokens[curTokenIdx++]; }

  const Token *pop() {
    consume();
    return &peek(-1);
  }

  bool consume() {
#if DEBUG
    if (this->debug) {
      void *array[3];
      std::string caller = backtrace_symbols(array, backtrace(array, 2))[1];
      if (caller.rfind("expect") < caller.size() ||
          caller.rfind("pop") < caller.size())
        caller = backtrace_symbols(array, backtrace(array, 3))[2];

      caller = caller.substr(caller.rfind("parse"));
      caller = caller.substr(0, caller.find("Ev"));

      std::cout << "\033[0;33m==> " << caller << " consumes \033[0;36m"
                << peek() << std::endl
                << "\033[0;37m";
      for (size_t i = curTokenIdx + 1, count = 0; i < tokens.size();
           (i++, count++)) {
        if (count > 5) {
          std::cout << "...";
          break;
        }
        std::cout << "[" << tokens[i] << "] ";
      }
      std::cout << "\033[0m" << std::endl;
    }
#endif
    curTokenIdx++;
    return true;
  }

  bool expect(TokenType tok) {
    if (peek().is(tok)) {
#if DEBUG
      if (this->debug) {
        void *array[2];
        std::string caller = backtrace_symbols(array, backtrace(array, 2))[1];
        caller = caller.substr(caller.rfind("parse"));
        caller = caller.substr(0, caller.find("Ev"));
        std::cout << "\033[0;31m==> " << caller << " expects \033[0;36m"
                  << peek().name() << "\033[0m ";
      }
#endif
      return consume();
    } else {
      error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
                           "Unexpected Token: \"" + peek().name() +
                               "\", expecting  \"" + (new Token(tok))->name() +
                               "\"");
      std::cout << error << std::endl;
      return false;
    }
    // FIXME get string version of token?
  }
#if DEBUG
  void printParserTrace() {
    if (this->debug) {
      void *array[2];
      std::string caller = backtrace_symbols(array, backtrace(array, 2))[1];
      caller = caller.substr(caller.rfind("parse"));
      caller = caller.substr(0, caller.rfind("Ev"));
      std::cout << "\033[0;37m" << peek() << "\033[0m -> " << caller
                << std::endl;
    }
  }
#endif

  const Token &peek(int k = 0) {
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
  // (6.9.1) function-definition :: type-specifier declarator
  // declaration+(opt) compound-statement (6.7)  declaration :: type-specifier
  // declarator(opt) ; (6.7.2) type-specifier :: void | char | short | int |
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
  //                   direct-abstract-declarator direct-abstract-declarator
  //                   :: ( abstract-declarator) | ( parameter-type-list(opt)
  //                   )+

  ASTNode *parseTranslationUnit();
  ASTNode *parseExternalDeclaration();
  ASTNode *parseFuncDefOrDeclaration();

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
  // (6.5.16) assignment-op: = | *= | /= | %= | += | -= | <<= | >>= | &= | ^=
  // |
  // |= (6.5.15) conditional: logical-OR | logical-OR ? expression :
  // conditional (6.5.14) logical-OR: logical-AND | logical-OR || logical-AND
  // (6.5.13) logical-AND: inclusive-OR | logical-AND && inclusive-OR
  // (6.5.12) inclusive-OR: exclusive-OR | inclusive-OR | exclusive-OR
  // (6.5.11) exclusive-OR: AND | exclusive-OR ^ AND
  // (6.5.10) AND: equality | AND & equality
  // (6.5.9) equality: relational | equality == relational | equality !=
  // relational (6.5.8) relational: shift | relational < shift | relational >
  // shift | relational <= shift | relational >= shift (6.5.7) shift: additive
  // | shift << additive | shift >> additive (6.5.6) additive: multiplicative
  // | additive + multiplicative | additive - multiplicative (6.5.5)
  // multiplicative: cast | multiplicative * cast | multiplicative / cast |
  // multiplicative % cast (6.5.4) cast: unary | ( type-name ) cast (6.5.3)
  // unary: postfix | ++ unary | -- unary | unary-op cast | sizeof unary |
  // sizeof ( type-name ) (6.5.3) unary-op: & | * | + | - | ~ |  ! (6.5.2)
  // postfix: primary | postfix [ expression ] | postfix (
  // argument-expr-list(opt) ) | postfix . identifier
  //                  postfix -> identifier | postfix -- | ( type-name ) {
  //                  initializer-list } | ( type-name ) { initializer-list ,
  //                  }
  // (6.5.2) argument-expr-list: assignment | argument-expr-list , assignment
  // (6.5.1) primary: identifer | constant | string-literal | ( expression )

  Expression *parseExpression();
  Expression *parseBinaryExpression(Expression *exp);
  Expression *parsePrimaryExpression();
  Expression *parseUnaryExpression();
  void parseArgumentExpressionList();
  Expression *parsePostfixExpression();

  // Statements
  Statement *parseStatement();
  Statement *parseCompoundStatement();
  Statement *parseBlockItemList();
  Statement *parseLabeledStatement();
  Statement *parseSelectionStatement();
  Statement *parseIterationStatement();

  std::vector<Token> &tokens;
  std::size_t curTokenIdx = 0;
  std::string error = "";
};

} // namespace ccc
#endif
