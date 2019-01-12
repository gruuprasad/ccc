#ifndef C4_PARSER_HPP
#define C4_PARSER_HPP

#include "../ast/ast_node.hpp"
#include "../lexer/fast_lexer.hpp"
#include "../lexer/token.hpp"
#include "../utils/assert.hpp"
#include "../utils/macros.hpp"
#include "../utils/utils.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <execinfo.h>
#include <iostream>
#include <vector>

namespace ccc {

enum PARSE_TYPE { TRANSLATIONUNIT, EXPRESSION, STATEMENT, DECLARATION };
constexpr static const std::size_t N = 3; // la_buffer size

class FastParser {
public:
  explicit FastParser(const std::string &content) : lexer(content) {
    for (auto &elem : la_buffer)
      elem = lexer.lex_valid();
  }

  std::unique_ptr<ASTNode> parse(PARSE_TYPE type = PARSE_TYPE::TRANSLATIONUNIT) {
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
      error_stream << "Unknown parse type [error appears only for unit testing]";
      return std::unique_ptr<TranslationUnit>();
    }
  }

  std::pair<unsigned long, unsigned long> getParserLocation() const {
    return lexer.getLexerLocation();
  }

  bool fail() const { return error_count != 0; }
  std::string getError() { return error_stream.str(); }
  
  void parser_error(const Token &tok) {
  error_count++;
  std::cerr << std::to_string(tok.getLine()) << ":" << std::to_string(tok.getColumn()) 
    << ": error:" << "Unexpected token " << tok.getExtra()  << ". Parsing Stopped!"
    << std::endl;
}

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
    parser_error(peek());
    return false;
  }

  const Token &peek(std::size_t k = 0) const {
    assert(k < N);
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

  std::unique_ptr<ASTNode> parseTranslationUnit();
  std::unique_ptr<Statement> parseExternalDeclaration();
  std::unique_ptr<Statement> parseFuncDefOrDeclaration();

  // Declarations
  std::unique_ptr<Statement> parseDeclaration();
  std::unique_ptr<TypeDeclaration> parseStructTypeDeclaration();
  std::unique_ptr<CompoundStatement> parseStructDefinition();
  std::unique_ptr<Expression> parseDeclarator();
  void parseParameterList();
  void parseParameterDeclaration();
  void parseStructMemberDeclaration();

  // Expressions
  std::unique_ptr<Expression> parseExpression();
  std::unique_ptr<Expression> parseAssignmentExpression();
  std::unique_ptr<Expression> parseBinOpWithRHS(std::unique_ptr<Expression>,
                                                Precedence minPrec);
  std::unique_ptr<Expression> parseUnaryExpression();
  std::unique_ptr<Expression> parsePostfixExpression();
  std::unique_ptr<Expression> parsePrimaryExpression();
  std::unique_ptr<Expression> parseArgumentExpressionList();

  // Statements
  std::unique_ptr<Statement> parseStatement();
  std::unique_ptr<Statement> parseCompoundStatement();
  void parseBlockItemList(); // XXX What is this?
  std::unique_ptr<Statement> parseLabeledStatement();
  std::unique_ptr<Statement> parseSelectionStatement();
  std::unique_ptr<Statement> parseIterationStatement();

  FastLexer lexer;
  std::array<Token, N> la_buffer;
  unsigned int error_count = 0;
  std::stringstream error_stream;
};

} // namespace ccc
#endif
