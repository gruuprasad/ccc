#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP

#include "../lexer/token.hpp"
#include "ast_node.hpp"
#include "expression.hpp"

#include <vector>

namespace ccc {

class Statement : public ASTNode {
private:
public:
  Statement(int id, const std::string name, unsigned long size = 0,
            const Token *token = nullptr, std::vector<ASTNode *> children = {})
      : ASTNode(id, name, size, token, children) {}

private:
  std::string toGraphWalker() override;
};

class LabelStatement : public Statement {
public:
  LabelStatement(int id, Expression *ident, Statement *stmt)
      : Statement(id, "labeled-statement", 2, nullptr, {ident, stmt}) {}
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(int id, const Token *token, std::vector<ASTNode *> items)
      : Statement(id, "compound-statement", 1, token, items) {}
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(int id, const Token *token,
                               Expression *expr = nullptr)
      : Statement(id, "expression-statement", expr == nullptr ? 0 : 1, token,
                  {expr}) {}
};

class IfStatement : public Statement {
public:
  IfStatement(int id, Expression *expr, Statement *stmt)
      : Statement(id, "selection-statement", 2, nullptr, {expr, stmt}) {}
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(int id, Expression *expr, Statement *stmt1, Statement *stmt2)
      : Statement(id, "selection-statement", 3, nullptr, {expr, stmt1, stmt2}) {
  }
};

class WhileStatement : public Statement {
public:
  WhileStatement(int, Expression *expr, Statement *stmt)
      : Statement(id, "iteration-statement", 2, nullptr, {expr, stmt}) {}
};

class GotoStatement : public Statement {
public:
  GotoStatement(int id, Expression *ident)
      : Statement(id, "jump-statement", 1, nullptr,
                  std::vector<ASTNode *>{ident}) {}
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(int id) : Statement(id, "jump-statement") {}
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(int id) : Statement(id, "jump-statement") {}
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(int id, Expression *expr)
      : Statement(id, "jump-statement", 1, nullptr,
                  std::vector<ASTNode *>{expr}) {}
};

} // namespace ccc
#endif // C4_STATEMENT_HPP
