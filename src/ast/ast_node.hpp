#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "type_specifier.hpp"
#include "../lexer/token.hpp"

#include <string>
#include <vector>
#include <sstream>


namespace ccc {

class ASTNode {
protected:
  ASTNode(int id, std::string name, Token *token = nullptr,
      std::vector<ASTNode *> children_ = std::vector<ASTNode *>())
      : id(id), name(std::move(name)), token(token),
        children(std::move(children_)) {}
  Token *token;
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

template <typename T>
static std::string graphWalkerImpl(int id, T constant) {
  std::stringstream ss;
  ss << id << "[label=\"" << constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

// AST nodes for constants

class Constant : public ASTNode {
public:
  explicit Constant(int id) : ASTNode(id, "constant") {}
};

class IntegerConstant : public Constant {
private:
  int constant;
  std::string toGraphWalker() override {
    return graphWalkerImpl(id, constant);
  }

public:
  IntegerConstant(int id, int constant) : Constant(id), constant(constant) {}
};

class CharacterConstant : public Constant {
private:
  char constant;
  std::string toGraphWalker() override {
    return graphWalkerImpl(id, constant);
  }

public:
  CharacterConstant(int id, char constant) : Constant(id), constant(constant) {}
};

class EnumerationConstant : public Constant {
private:
  std::string constant;
  std::string toGraphWalker() override {
    return graphWalkerImpl(id, constant);
  }

public:
  EnumerationConstant(int id, std::string &constant)
      : Constant(id), constant(constant) {}
};

// AST nodes for declarations

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

// AST nodes for expression

class Expression : public ASTNode {
private:
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name << "<br/><font point-size='10'>"
      << this->name
      << "</font>> shape=oval style=filled fillcolor=lightskyblue];\n";
    for (ASTNode *child : this->children) {
      ss << child->toGraphWalker();
      ss << this->id << " -- " << child->getId()
        << "[taillabel=\"?\" labeldistance=0 labelangle=0 "
        "labelfontcolor=red];\n";
    }
    return ss.str();
  }

public:
  Expression(int id, std::string name, Token *token = nullptr, 
              std::vector<ASTNode *> children = {})
      : ASTNode(id, name, token, children) {}
};

class Identifier : public Expression {
public:
  explicit Identifier(int id) : Expression(id, "primary-expression") {}
};

class StringLiteral : public Expression {};

class PrimaryExpression : public Expression {};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(int id, Expression *assign, Expression *expression)
      : Expression(id, "assignment-expression", nullptr, {assign, expression})
      {}
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
  MultiplicativeExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "multiplicative-expression", nullptr,
                   {expr1, expr2}) {}
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int, Expression *expr1, Expression *expr2)
      : Expression(id, "additive-expression", nullptr, {expr1, expr2}) {}
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "relational-expression", nullptr, {expr1, expr2}) {}
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "equality-expression", nullptr, {expr1, expr2}) {}
};

class LogicalAndExpression : public Expression {
public:
  LogicalAndExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-And-expression", nullptr, {expr1, expr2}) {}
};

class LogicalOrExpression : public Expression {
public:
  LogicalOrExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-Or-expression", nullptr, {expr1, expr2}) {}
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

// AST nodes for statements

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

#endif // C4_ASTNODE_HPP
