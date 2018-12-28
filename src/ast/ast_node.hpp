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

public:
  unsigned long getId() const { return id; }
  virtual std::string graphWalker() = 0;
  virtual std::string prettyPrint() { return this->prettyPrint(0); };
  virtual std::string prettyPrint(int) { return "\n?"; };
  virtual std::string prettyPrintInline(int lvl) {
    return this->prettyPrint(lvl);
  };
  virtual std::string prettyPrintInlineElse(int lvl) {
    return this->prettyPrintInline(lvl);
  };
  virtual std::string prettyPrintInlineIf(int lvl) {
    return this->prettyPrintInline(lvl);
  };
  virtual std::string checkSemantic() { return "?"; };
  virtual std::string toCode() { return "?"; };
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->graphWalker() + "}\n}\n";
  }
  virtual ~ASTNode() {
    for (auto &child : this->children) {
      delete child;
    }
  }
  template <class C> bool instanceof () { return (dynamic_cast<C *>(this)); }
};

enum class TypeSpecifier { VOCHAR, INT, STRUCT };

class GhostNode : public ASTNode {
private:
public:
  GhostNode(const Token *token, ASTNode *child)
      : ASTNode("ghost", token, {child}) {}

  explicit GhostNode(const Token *token, std::vector<ASTNode *> children = {})
      : ASTNode("ghost", token, std::move(children)) {}

  std::string prettyPrint(int lvl) override {
    std::stringstream ss;
    for (ASTNode *child : this->children)
      ss << child->prettyPrint(lvl);
    return ss.str();
  }

private:
  std::string graphWalker() override {
    if (children.empty() && this->token) {
      std::stringstream ss;
      ss << this->id << "[label=<<font point-size='10'>" << *this->token
         << "</font>";
      ss << "> shape=none style=filled fillcolor=lightgrey];\n";
      return ss.str();
    } else {
      std::stringstream ss;
      for (ASTNode *child : children) {
        ss << child->graphWalker();
      }
      return ss.str();
    }
  }
};

class Declaration : public ASTNode {
private:
  ASTNode *ident;
  TypeSpecifier type;
  std::string graphWalker() override {
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
  std::string graphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name;
    if (this->token)
      ss << "<br/><font point-size='10'>" << *this->token << "</font>";
    ;
    ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
    for (ASTNode *child : this->children) {
      if (child) {
        ss << child->graphWalker() << this->id << "--" << child->getId()
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

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(const Token *token)
      : Expression("primary expression", token, {}){};
  explicit PrimaryExpression(std::string name, const Token *token)
      : Expression(std::move(name), token, {}){};

private:
  std::string graphWalker() override {
    std::stringstream ss;
    ss << id << "[label=<" << this->token->getExtra()
       << "> shape=diamond style=filled fillcolor=lightyellow];\n";
    return ss.str();
  }
  std::string prettyPrint(int) override { return this->token->getExtra(); }
};

class Identifier : public PrimaryExpression {
public:
  explicit Identifier(const Token *token = nullptr)
      : PrimaryExpression("identifier", token) {}
};

class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token *token = nullptr)
      : PrimaryExpression("string-literal", token) {}
};

class Constant : public PrimaryExpression {
public:
  explicit Constant(const Token *token = nullptr)
      : PrimaryExpression("constant", token) {}
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
  BinaryExpression(const Token *token, Expression *expr1, Expression *expr2)
      : Expression("additive-expression", token, {expr1, expr2}) {}
  std::string prettyPrint() override {
    return "(" + children[0]->prettyPrint() + " " + token->name() + " " +
           children[1]->prettyPrint() + ")";
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
  std::string graphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=<" << this->name;
    if (this->token)
      ss << "<br/><font point-size='10'>" << *this->token << "</font>";
    ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
    for (ASTNode *child : this->children) {
      if (child) {
        ss << "subgraph cluster_" << child->getId() << "{\n"
           << child->graphWalker() << "}\n";
        ss << this->id << "--" << child->getId() << ";\n";
      }
    }
    return ss.str();
  }

protected:
  std::string indent(int n) {
    if (n >= 0) {
      std::stringstream ss;
      for (int i = 0; i < n; i++)
        ss << "\t";
      return ss.str();
    } else
      return "";
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
  std::string prettyPrint(int lvl) override {
    std::stringstream ss;
    for (ASTNode *child : this->children) {
      ss << child->prettyPrint(lvl + 1);
    }
    return indent(lvl) + "{\n" + ss.str() + indent(lvl) + "}\n";
  }
  std::string prettyPrintInline(int lvl) override {
    std::stringstream ss;
    for (ASTNode *child : this->children) {
      ss << child->prettyPrint(lvl);
    }
    return " {\n" + ss.str() + indent(lvl - 1) + "}\n";
  }
  std::string prettyPrintInlineElse(int lvl) override {
    std::stringstream ss;
    for (ASTNode *child : this->children) {
      ss << child->prettyPrint(lvl);
    }
    return " {\n" + ss.str() + indent(lvl - 1) + "} ";
  }
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(const Token *token, Expression *expr = nullptr)
      : Statement("expression-statement", expr == nullptr ? nullptr : token,
                  {expr}) {}
  std::string prettyPrint(int lvl) override {
    return indent(lvl) + children[0]->prettyPrint() + ";\n";
  };
  std::string prettyPrintInline(int lvl) override {
    return "\n" + indent(lvl) + children[0]->prettyPrint() + ";\n";
  }
  std::string prettyPrintInlineElse(int lvl) override {
    return "\n" + indent(lvl) + children[0]->prettyPrint() + ";\n" +
           indent(lvl - 1);
  }
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(const Token *token, Expression *expr, Statement *stmt1,
                  Statement *stmt2 = nullptr)
      : Statement("selection-statement", token, {expr, stmt1, stmt2}) {}
  std::string prettyPrint(int lvl) override {
    std::stringstream ss;
    if (this->children[2]) {
      ss << indent(lvl) << "if (" << this->children[0]->prettyPrint() << ")"
         << this->children[1]->prettyPrintInlineElse(lvl + 1);
      ss << "else" << this->children[2]->prettyPrintInlineIf(lvl + 1);
    } else {
      ss << indent(lvl) << "if (" << this->children[0]->prettyPrint() << ")"
         << this->children[1]->prettyPrintInline(lvl + 1);
    }
    return ss.str();
  }
  std::string prettyPrintInline(int lvl) override {
    return "\n" + this->prettyPrint(lvl);
  }
  std::string prettyPrintInlineIf(int lvl) override {
    std::stringstream ss;
    if (this->children[2]) {
      ss << " if (" << this->children[0]->prettyPrint() << ")"
         << this->children[1]->prettyPrintInlineElse(lvl);
      ss << "else" << this->children[2]->prettyPrintInline(lvl);
    } else {
      ss << " if (" << this->children[0]->prettyPrint() << ")"
         << this->children[1]->prettyPrintInline(lvl);
    }
    return ss.str();
  }
};

class WhileStatement : public Statement {
public:
  WhileStatement(const Token *token, Expression *expr, Statement *stmt)
      : Statement("iteration-statement", token, {expr, stmt}) {}
  std::string prettyPrint(int lvl) override {
    std::stringstream ss;
    ss << indent(lvl) << "while (" << this->children[0]->prettyPrint() << ")"
       << this->children[1]->prettyPrintInline(lvl + 1);
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
  std::string prettyPrint(int lvl) override { return indent(lvl) + "break;"; }
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string prettyPrint(int lvl) override {
    return indent(lvl) + "continue;";
  }
};

class ReturnStatement : public Statement {
public:
  explicit ReturnStatement(const Token *token, Expression *expr = nullptr)
      : Statement("jump-statement", token, std::vector<ASTNode *>{expr}) {}
  std::string prettyPrint(int lvl) override {
    if (children[0])
      return indent(lvl) + "return " + children[0]->prettyPrint() + ";\n";
    else
      return indent(lvl) + "return;\n";
  };
  std::string prettyPrintInline(int lvl) override {
    return "\n" + this->prettyPrint(lvl);
  }
  std::string prettyPrintInlineElse(int lvl) override {
    return this->prettyPrintInline(lvl) + indent(lvl - 1);
  }
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
