#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace ccc {

class ASTNode {
protected:
  ASTNode(int id, std::string name, const Token *token = nullptr,
          std::vector<ASTNode *> children = std::vector<ASTNode *>())
      : id(id), name(std::move(name)), token(token),
        children(std::move(children)) {}
  const Token *token;
  int id;
  std::string name;
  std::vector<ASTNode *> children;

public:
  int getId() const { return id; }
  virtual std::string toGraphWalker() = 0;
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->toGraphWalker() + "}\n}\n";
  }
  virtual ~ASTNode() {
    for (auto &child : this->children) {
      delete child;
    }
  }
};

enum class TypeSpecifier { VOID, CHAR, INT, STRUCT };

class Ghost : public ASTNode {
private:
public:
  Ghost(int id, const Token *token, ASTNode *child)
      : ASTNode(id, "ghost", token, {child}) {}

  Ghost(int id, const Token *token, std::vector<ASTNode *> children = {})
      : ASTNode(id, "ghost", token, children) {}

private:
  std::string toGraphWalker() override {
    if (children.empty() && this->token) {
      std::stringstream ss;
      ss << this->id << "[label=<<font point-size='10'>" << *this->token
         << "</font>";
      ss << "> shape=none style=filled fillcolor=lightgrey];\n";
      return ss.str();
    } else {
      std::stringstream ss;
      for (ASTNode *child : children) {
        ss << child->toGraphWalker();
      }
      return ss.str();
    }
  }
};

template <typename T> static std::string graphWalkerImpl(int id, T constant) {
  std::stringstream ss;
  ss << id << "[label=<" << constant
     << "> shape=diamond style=filled fillcolor=lightyellow];\n";
}

class Constant : public ASTNode {
public:
  explicit Constant(int id) : ASTNode(id, "constant") {}
};

class IntegerConstant : public Constant {
  int constant;
  std::string toGraphWalker() override { return graphWalkerImpl(id, constant); }

public:
  IntegerConstant(int id, int constant) : Constant(id), constant(constant) {}
};

class CharacterConstant : public Constant {
private:
  char constant;
  std::string toGraphWalker() override { return graphWalkerImpl(id, constant); }

public:
  CharacterConstant(int id, char constant) : Constant(id), constant(constant) {}
};

class EnumerationConstant : public Constant {
private:
  std::string constant;
  std::string toGraphWalker() override { return graphWalkerImpl(id, constant); }

public:
  EnumerationConstant(int id, std::string &constant)
      : Constant(id), constant(constant) {}
};

class Declaration : public ASTNode {
private:
  ASTNode *ident;
  TypeSpecifier type;
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=\"" << this->name
       << "\" shape=box style=filled fillcolor=lightsalmon];\n";
    return ss.str();
  }

public:
  Declaration(int id, ASTNode *ident, TypeSpecifier type)
      : ASTNode(id, "declaration"), ident(ident), type(type) {}
};

class InitDeclaration : Declaration {
public:
  InitDeclaration(int id, ASTNode *ident, TypeSpecifier type)
      : Declaration(id, ident, type) {}
};

class Expression : public ASTNode {
private:
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name;
    if (this->token)
      ss << "<br/><font point-size='10'>" << *this->token << "</font>";
    ;
    ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
    for (ASTNode *child : this->children) {
      if (child) {
        ss << child->toGraphWalker() << this->id << " -- " << child->getId()
           << std::endl;
      }
    }
    return ss.str();
  }

public:
  Expression(int id, std::string name, const Token *token = nullptr,
             std::vector<ASTNode *> children = {})
      : ASTNode(id, name, token, children) {}
};

class Identifier : public Expression {
public:
  explicit Identifier(int id) : Expression(id, "primary-expression", 0) {}
};

class StringLiteral : public Expression {};

class PrimaryExpression : public Expression {
public:
  PrimaryExpression(int id, const Token *token)
      : Expression(id, "primary expression", token, {}){};
};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(int id, Expression *assign, const Token *token,
                       Expression *expression)
      : Expression(id, "assignment-expression", token, {assign, expression}) {}
};

class FunctionCall : public Expression {
public:
  FunctionCall(int id, std::vector<ASTNode *> arguments)
      : Expression(id, "postfix-expression", nullptr, arguments) {}
};

class SizeOfExpression : public Expression {
public:
  SizeOfExpression(int id, ASTNode *type)
      : Expression(id, "unary-expression", nullptr,
                   std::vector<ASTNode *>{type}) {}
};

class MultiplicativeExpression : public Expression {
public:
  MultiplicativeExpression(int id, Expression *expr1, const Token *token,
                           Expression *expr2)
      : Expression(id, "multiplicative-expression", token, {expr1, expr2}) {}
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int, Expression *expr1, const Token *token,
                     Expression *expr2)
      : Expression(id, "additive-expression", token, {expr1, expr2}) {}
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int id, Expression *expr1, const Token *token,
                       Expression *expr2)
      : Expression(id, "relational-expression", token, {expr1, expr2}) {}
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int id, Expression *expr1, const Token *token,
                     Expression *expr2)
      : Expression(id, "equality-expression", token, {expr1, expr2}) {}
};

class LogicalAndExpression : public Expression {
public:
  LogicalAndExpression(int id, Expression *expr1, const Token *token,
                       Expression *expr2)
      : Expression(id, "logical-And-expression", token, {expr1, expr2}) {}
};

class LogicalOrExpression : public Expression {
public:
  LogicalOrExpression(int id, Expression *expr1, const Token *token,
                      Expression *expr2)
      : Expression(id, "logical-Or-expression", token, {expr1, expr2}) {}
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(int id, Expression *expr1, Expression *expr2,
                        Expression *expr3)
      : Expression(id, "conditional-expression", nullptr,
                   {expr1, expr2, expr3}) {}
};

class ConstantExpression : public Expression {
public:
  ConstantExpression(int id, Constant *constant)
      : Expression(id, "primary-expression", nullptr,
                   std::vector<ASTNode *>{constant}) {}
};

class Statement : public ASTNode {
private:
public:
  Statement(int id, const std::string name, const Token *token = nullptr,
            std::vector<ASTNode *> children = {})
      : ASTNode(id, name, token, children) {}

private:
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name;
    if (this->token)
      ss << "<br/><font point-size='10'>" << *this->token << "</font>";
    ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
    for (ASTNode *child : this->children) {
      if (child) {
        ss << "subgraph cluster_" << child->getId() << "{\n"
           << child->toGraphWalker() << "}\n";
        ss << this->id << "--" << child->getId() << ";\n";
      }
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
  CompoundStatement(int id, const Token *token, std::vector<ASTNode *> items)
      : Statement(id, "compound-statement", token, items) {}
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(int id, const Token *token,
                               Expression *expr = nullptr)
      : Statement(id, "expression-statement", expr == nullptr ? 0 : token,
                  {expr}) {}
};

class IfStatement : public Statement {
public:
  IfStatement(int id, Expression *expr, Statement *stmt)
      : Statement(id, "selection-statement", nullptr, {expr, stmt}) {}
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(int id, Expression *expr, Statement *stmt1, Statement *stmt2)
      : Statement(id, "selection-statement", nullptr, {expr, stmt1, stmt2}) {}
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
      : Statement(id, "jump-statement", nullptr, std::vector<ASTNode *>{expr}) {
  }
};

// AST nodes for expression
} // namespace ccc

#endif // C4_ASTNODE_HPP
