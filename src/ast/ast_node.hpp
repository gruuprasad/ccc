#include <utility>

#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ccc {

class ASTNode {
protected:
  unsigned long id;
  std::string name;
  const Token *token;
  std::vector<ASTNode *> children;
  explicit ASTNode(std::string name, const Token *token = nullptr,
                   std::vector<ASTNode *> children = std::vector<ASTNode *>())
      : id((unsigned long)this), name(std::move(name)), token(token),
        children(std::move(children)) {}
  std::string feed(int n) {
    std::stringstream ss;
    for (int i = 0; i < n; i++)
      ss << ". ";
    return ss.str();
  }

public:
  unsigned long getId() const { return id; }
  virtual std::string toGraphWalker() = 0;
  std::string toString() { return this->toString(0); };
  virtual std::string toString(int) { return "?"; };
  virtual std::string checkSemantic() { return "?"; };
  virtual std::string toCode() { return "?"; };
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->toGraphWalker() + "}\n}\n";
  }
  virtual ~ASTNode() {
    for (auto &child : this->children) {
      delete child;
    }
  }
  template <class C> bool instanceof () { return (dynamic_cast<C *>(this)); }
};

enum class TypeSpecifier { VOCHAR, INT, STRUCT };

class Ghost : public ASTNode {
private:
public:
  Ghost(const Token *token, ASTNode *child)
      : ASTNode("ghost", token, {child}) {}

  explicit Ghost(const Token *token, std::vector<ASTNode *> children = {})
      : ASTNode("ghost", token, std::move(children)) {}

  std::string toString(int) override {
    std::stringstream ss;
    for (ASTNode *child : this->children)
      ss << child->toString();
    return ss.str();
  }

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

class Constant : public ASTNode {
protected:
  template <typename T> std::string graphWalkerImpl(T constant) {
    std::stringstream ss;
    ss << id << "[label=<" << constant
       << "> shape=diamond style=filled fillcolor=lightyellow];\n";
    return ss.str();
  }

public:
  explicit Constant() : ASTNode("constant") {}
};

class IntegerConstant : public Constant {
  int constant;
  std::string toGraphWalker() override { return graphWalkerImpl(constant); }

public:
  explicit IntegerConstant(int constant) : Constant(), constant(constant) {}
};

class CharacterConstant : public Constant {
private:
  char constant;
  std::string toGraphWalker() override { return graphWalkerImpl(constant); }

public:
  explicit CharacterConstant(char constant) : Constant(), constant(constant) {}
};

class EnumerationConstant : public Constant {
private:
  std::string constant;
  std::string toGraphWalker() override { return graphWalkerImpl(constant); }

public:
  explicit EnumerationConstant(std::string &constant)
      : Constant(), constant(constant) {}
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
  Declaration(ASTNode *ident, TypeSpecifier type)
      : ASTNode("declaration"), ident(ident), type(type) {}
};

class InitDeclaration : Declaration {
public:
  InitDeclaration(ASTNode *ident, TypeSpecifier type)
      : Declaration(ident, type) {}
};

// AST nodes for expression

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
        ss << child->toGraphWalker() << this->id << "--" << child->getId()
           << std::endl;
      }
    }
    return ss.str();
  }

public:
  explicit Expression(std::string name, const Token *token = nullptr,
                      std::vector<ASTNode *> children = {})
      : ASTNode(std::move(name), token, std::move(children)) {}
};

class Identifier : public Expression {
public:
  explicit Identifier() : Expression("primary-expression", nullptr) {}
};

class StringLiteral : public Expression {};

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(const Token *token)
      : Expression("primary expression", token, {}){};
  std::string toString(int indent) override {
    return feed(indent) + this->token->getExtra();
  };
};

class FunctionCall : public Expression {
public:
  explicit FunctionCall(std::vector<ASTNode *> arguments)
      : Expression("postfix-expression", nullptr, std::move(arguments)) {}
};

class SizeOfExpression : public Expression {
public:
  explicit SizeOfExpression(ASTNode *type)
      : Expression("unary-expression", nullptr, std::vector<ASTNode *>{type}) {}
};

class BinaryExpression : public Expression {
public:
  BinaryExpression(Expression *expr1, const Token *token, Expression *expr2)
      : Expression("additive-expression", token, {expr1, expr2}) {}
  std::string toString(int indent) override {
    return feed(indent) + children[0]->toString() + " " + token->name() + " " +
           children[1]->toString();
  };
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(Expression *expr1, Expression *expr2, Expression *expr3)
      : Expression("conditional-expression", nullptr, {expr1, expr2, expr3}) {}
};

class ConstantExpression : public Expression {
public:
  explicit ConstantExpression(Constant *constant)
      : Expression("primary-expression", nullptr,
                   std::vector<ASTNode *>{constant}) {}
};

// AST nodes for statements

class Statement : public ASTNode {
private:
public:
  explicit Statement(const std::string &name, const Token *token = nullptr,
                     std::vector<ASTNode *> children = {})
      : ASTNode(name, token, std::move(children)) {}

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
  LabelStatement(Expression *ident, Statement *stmt)
      : Statement("labeled-statement", nullptr, {ident, stmt}) {}
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(const Token *token, std::vector<ASTNode *> items)
      : Statement("compound-statement", token, std::move(items)) {}
  std::string toString(int indent) override {
    std::stringstream ss;
    for (ASTNode *child : this->children) {
      ss << feed(indent + 1) << child->toString(indent + 1) << std::endl;
    }
    return "{\n" + ss.str() + feed(indent) + "}";
  }
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(const Token *token, Expression *expr = nullptr)
      : Statement("expression-statement", expr == nullptr ? nullptr : token,
                  {expr}) {}
  std::string toString(int) override { return children[0]->toString() + ";"; };
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(const Token *token, Expression *expr, Statement *stmt1,
                  Statement *stmt2 = nullptr)
      : Statement("selection-statement", token, {expr, stmt1, stmt2}) {}
  std::string toString(int indent) override {
    std::stringstream ss;
    ss << "if (" << this->children[0]->toString() << ") "
       << this->children[1]->toString(indent);
    if (this->children[2])
      ss << std::endl
         << feed(indent) << "else " << this->children[2]->toString(indent);
    return ss.str();
  }
};

class WhileStatement : public Statement {
public:
  WhileStatement(const Token *token, Expression *expr, Statement *stmt)
      : Statement("iteration-statement", token, {expr, stmt}) {}
  std::string toString(int indent) override {
    std::stringstream ss;
    ss << "while (" << this->children[0]->toString() << ") "
       << this->children[1]->toString(indent);
    return ss.str();
  }
};

class GotoStatement : public Statement {
public:
  explicit GotoStatement(Expression *ident)
      : Statement("jump-statement", nullptr, std::vector<ASTNode *>{ident}) {}
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string toString(int) override { return "break;"; }
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string toString(int) override { return "continue;"; }
};

class ReturnStatement : public Statement {
public:
  explicit ReturnStatement(const Token *token, Expression *expr = nullptr)
      : Statement("jump-statement", token, std::vector<ASTNode *>{expr}) {}
  std::string toString(int) override {
    std::stringstream ss;
    ss << "return";
    if (this->children[0])
      ss << "(" << this->children[0]->toString() + ")";
    return ss.str() + ";";
  }
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
