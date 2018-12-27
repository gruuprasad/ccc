#ifndef C4_PARSER_HPP
#define C4_PARSER_HPP

#include "../lexer/fast_lexer.hpp"
#include "../lexer/token.hpp"
#include "../utils/assert.hpp"
#include "../utils/macros.hpp"
#include <algorithm>
#include <cassert>
#include <vector>
#include <array>

namespace ccc {

enum PARSE_TYPE { TRANSLATIONUNIT, EXPRESSION, STATEMENT, DECLARATION };
constexpr static const std::size_t N = 3; //la_buffer size

class FastParser {
public:
  explicit FastParser(const std::string & content) : lexer(content) {
    for (auto & elem : la_buffer)
      elem = lexer.lex_valid();
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
      return parseDeclaration();
    default:
      error = "Unknown parse type";
    }
  }

  bool fail() const { return !error.empty(); }
  std::string getError() { return error; }

private:
  Token nextToken() {
    auto ret = la_buffer.front();
    std::rotate(la_buffer.begin(), la_buffer.begin() + 1, la_buffer.end());
    la_buffer.back() = lexer.lex_valid();
    return ret;
  }

  bool mayExpect(TokenType tok_type) {
    if (peek().is(tok_type)) {
      nextToken();
      return true;
    }
    return false;
  }


  bool mustExpect(TokenType tok_type) {
    if (peek().is(tok_type)) {
      nextToken(); // Token is not used by parser
      return true;
    }
    
    error = PARSER_ERROR(peek().getLine(), peek().getColumn(),
        "Unexpected Token: \"" + peek().name() + "\", expecting  \"" + "\"");
    return false;
  }

  const Token &peek(std::size_t k = 0) const {
    assert (k < N);
    return la_buffer[k];
  }

  template <typename F> void parseList(F word, TokenType delimit) {
    do {
      word();
    } while (mayExpect(delimit));
  }

  template <typename Func> void parseParenthesizedFn(Func f) {
    mustExpect(TokenType::PARENTHESIS_OPEN);
    f();
    mustExpect(TokenType::PARENTHESIS_CLOSE);
  }

  void parseTranslationUnit();
  void parseExternalDeclaration();
  void parseFuncDefOrDeclaration();

  // Declarations
  void parseDeclaration();
  void parseTypeSpecifier();
  void parseDeclarator();
  void parseParameterList();
  void parseParameterDeclaration();
  void parseStructOrUnionSpecifier();
  void parseStructDeclaration();

  // Expressions
  void parseExpression();
  void parseAssignmentExpression();
  void parseBinOpWithRHS(/* LHS ,*/ Precedence minPrec);
  void parseUnaryExpression();
  void parsePostfixExpression();
  void parsePrimaryExpression();
  void parseArgumentExpressionList();

  // Statements
  void parseCompoundStatement();
  void parseBlockItemList();
  void parseStatement();
  void parseLabeledStatement();
  void parseSelectionStatement();
  void parseIterationStatement();

  FastLexer lexer;
  std::array<Token, N> la_buffer;
  std::string error = "";
};

} // namespace ccc
#endif
