#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP


#include "../lexer/token.hpp"
#include "ast_node.hpp"
#include "expression.hpp"

#include <vector>
#include <sstream>

namespace ccc {

class Statement : public ASTNode {
private:
public:
  Statement(int id, const std::string name, Token *token = nullptr, 
      std::vector<ASTNode *> children = {})
      : ASTNode(id, name, token, children) {}

private:
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name << "<br/><font point-size='10'>"
      << this->name
      << "</font>> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
    for (ASTNode *child : this->children) {
      ss << "subgraph cluster_" << child->getId() << "{\n"
        << child->toGraphWalker() << "}\n";
      ss << this->id << "--" << child->getId() << ";\n";
    }
    return ss.str();
  }
};

class LabelStatement : public Statement {
public:
  LabelStatement(int id, Expression *ident, Statement *stmt)
      : Statement(id, "labeled-statement", nullptr, {ident, stmt}) {}
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(int id, std::vector<ASTNode *> items)
      : Statement(id, "compound-statement", nullptr, items) {}
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(int id, Expression *expr = nullptr)
      : Statement(id, "expresson-statement") {}
};

class IfStatement : public Statement {
public:
  IfStatement(int id, Expression *expr, Statement *stmt)
      : Statement(id, "selection-statement", nullptr, {expr, stmt}) {}
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(int id, Expression *expr, Statement *stmt1, Statement *stmt2)
      : Statement(id, "selection-statement", nullptr, {expr, stmt1, stmt2}) {
  }
};

class WhileStatement : public Statement {
public:
  WhileStatement(int, Expression *expr, Statement *stmt)
      : Statement(id, "iteration-statement", nullptr, {expr, stmt}) {}
};

class GotoStatement : public Statement {
public:
  GotoStatement(int id, Expression *ident)
      : Statement(id, "jump-statement", nullptr,
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
      : Statement(id, "jump-statement", nullptr,
                  std::vector<ASTNode *>{expr}) {}
};

} // namespace ccc
#endif // C4_STATEMENT_HPP
