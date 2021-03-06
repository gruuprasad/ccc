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
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace ccc {

enum PARSE_TYPE { TRANSLATIONUNIT, EXPRESSION, STATEMENT, DECLARATION };
constexpr static const std::size_t N = 3; // la_buffer size

class FastParser {
  std::string filename;
  bool abstract = false;
  Token abstract_loc = Token();

public:
  explicit FastParser(const std::string &content, std::string f = "")
      : filename(std::move(f)), lexer(content, filename) {
    for (auto &elem : la_buffer)
      elem = lexer.lex_valid();
  }

  std::unique_ptr<ASTNode>
  parse(PARSE_TYPE type = PARSE_TYPE::TRANSLATIONUNIT) {
    switch (type) {
    case PARSE_TYPE::TRANSLATIONUNIT:
      return parseTranslationUnit();
    case PARSE_TYPE::EXPRESSION:
      return parseExpression();
    case PARSE_TYPE::STATEMENT:
      return parseStatement();
    case PARSE_TYPE::DECLARATION:
      return parseFuncDefOrDeclaration();
    default:
      error_stream
          << "Unknown parse type [error appears only for unit testing]";
      return std::unique_ptr<TranslationUnit>();
    }
  }

  bool fail() const { return !error.empty(); }
  std::string getError() { return error; }

  void log_msg(const Token &tok, const std::string &msg = std::string()) {
    std::cout << msg << " peek() token info = ";
    std::cout << std::to_string(tok.getLine()) << ":"
              << std::to_string(tok.getColumn()) << ":" << tok.name() << ":\n";
  }

  void parser_error(const Token &tok, const std::string &msg = std::string()) {
    if (tok.getType() == TokenType::INVALIDTOK)
      error = lexer.getError();
    else {
      if (!error.empty())
        error += "\n";
      error += (filename.empty() ? filename : filename + ":") +
               std::to_string(tok.getLine()) + ":" +
               std::to_string(tok.getColumn()) + ": error:";
      if (!msg.empty())
        error += " Expected " + msg + " found \"" + tok.name() + "\".";
      else
        error += " Unexpected Token \"" + tok.name() + "\" found.";

      error += " Parsing Stopped!";
    }
  }

private:
  Token nextToken() {
    auto ret = la_buffer.front();
    std::rotate(la_buffer.begin(), la_buffer.begin() + 1, la_buffer.end());
    la_buffer.back() = lexer.lex_valid();
    return ret;
  }

  void consume(const TokenType) { nextToken(); }

  bool mayExpect(const TokenType tok_type) {
    if (peek().is(tok_type)) {
      nextToken();
      return true;
    }
    return false;
  }

  bool mustExpect(const TokenType tok_type,
                  const std::string &msg = std::string()) {
    if (peek().is(tok_type)) {
      nextToken(); // Token is not used by parser
      return true;
    }
    parser_error(peek(), msg);
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

  std::unique_ptr<TranslationUnit> parseTranslationUnit();
  std::unique_ptr<ExternalDeclaration> parseExternalDeclaration();
  std::unique_ptr<ExternalDeclaration> parseFuncDefOrDeclaration();
  std::unique_ptr<ExternalDeclaration> parseDeclaration();

  // Declarations
  std::pair<std::unique_ptr<Type>, bool> parseTypeSpecifier();
  std::pair<std::unique_ptr<StructType>, bool> parseStructType();
  std::unique_ptr<Declarator> parseDeclarator(bool within_paren = false);
  std::unique_ptr<Declarator> parseDirectDeclarator(bool in_paren,
                                                    int ptrCount);
  ParamDeclarationListType parseParameterList();
  std::unique_ptr<ParamDeclaration> parseParameterDeclaration();

  // Expressions
  std::unique_ptr<Expression> parseExpression();
  std::unique_ptr<Expression> parseAssignmentExpression();
  std::unique_ptr<Expression> parseBinOpWithRHS(std::unique_ptr<Expression> lhs,
                                                Precedence minPrec);
  std::unique_ptr<Expression> parseUnaryExpression();
  std::unique_ptr<Expression> parsePostfixExpression();
  std::unique_ptr<Expression> parsePrimaryExpression();
  ExpressionListType parseArgumentExpressionList();

  // Statements
  std::unique_ptr<Statement> parseStatement();
  std::unique_ptr<Statement> parseCompoundStatement();
  std::unique_ptr<Statement> parseLabeledStatement();
  std::unique_ptr<Statement> parseSelectionStatement();
  std::unique_ptr<Statement> parseIterationStatement();

  FastLexer lexer;
  std::array<Token, N> la_buffer;
  std::string error;
  std::stringstream error_stream;
  // Variables to hold certain states during parsing.
  bool isFunctionIdentifer = false;
  //  bool fnReturnTypeWithPtr = false;
  //  unsigned int ignoredPtrCount = 0;
  Token global_mark;
}; // namespace ccc

} // namespace ccc
#endif
