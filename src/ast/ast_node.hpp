#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace ccc {

enum class TypeSpecifier { VOID, CHAR, INT, STRUCT };

class ASTNode {
protected:
  std::string name; // Text representing type of the AST node.
  const Token *token;
  std::vector<ASTNode *> children;
  explicit ASTNode(std::string name, const Token *token = nullptr)
      : name(std::move(name)), token(token),
        children(std::vector<ASTNode *>()) {}

public:
  virtual std::string graphWalker() = 0;
  virtual std::string prettyPrint() { return this->prettyPrint(0); };
  virtual std::string prettyPrint(int) { return "-?-"; };
  virtual std::string checkSemantic() { return "?"; };
  virtual std::string toCode() { return "?"; };
  template <class C> C *cast() { return dynamic_cast<C *>(this); }
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->graphWalker() + "}\n}\n";
  }

  virtual ~ASTNode() {
    delete token;
    for (auto &child : this->children) {
      delete child;
    }
  }
  template <class C> bool instanceof () { return (dynamic_cast<C *>(this)); }
};

class TranslationUnit : public ASTNode {
private:
public:
  TranslationUnit(const Token *token, ASTNode *child)
      : ASTNode("translationUnit", token) {
    this->children.push_back(child);
  }

  TranslationUnit(const Token *token) : ASTNode("translationUnit", token) {
    this->children.insert(this->children.end(), children.begin(),
                          children.end());
  }

  std::string prettyPrint(int lvl) override;

private:
  std::string graphWalker() override;
};

class Declaration : public ASTNode {
public:
  Declaration(ASTNode *ident, TypeSpecifier type)
      : ASTNode("declaration"), ident(ident), type(type) {}

private:
  std::string graphWalker() override;
  ASTNode *ident;
  TypeSpecifier type;
};

class InitDeclaration : Declaration {
public:
  InitDeclaration(ASTNode *ident, TypeSpecifier type)
      : Declaration(ident, type) {}
};

// AST nodes for expression
class Expression : public ASTNode {
public:
  explicit Expression(std::string name, const Token *token = nullptr)
      : ASTNode(std::move(name), token) {}

private:
  std::string graphWalker() override;
};

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(const Token *token)
      : Expression("primary expression", token){};
  PrimaryExpression(std::string name, const Token *token)
      : Expression(std::move(name), token){};
  std::string prettyPrint(int) override { return this->token->getExtra(); }

private:
  std::string graphWalker() override;
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
      : Expression("postfix-expression") {
    this->children.insert(this->children.end(), arguments.begin(),
                          arguments.end());
  }
};

class SizeOfExpression : public Expression {
public:
  explicit SizeOfExpression(ASTNode *type) : Expression("unary-expression") {
    this->children.push_back(type);
  }
};

class BinaryExpression : public Expression {
public:
  BinaryExpression(const Token *token, Expression *expr1, Expression *expr2)
      : Expression("additive-expression", token) {
    this->children.insert(this->children.end(), {expr1, expr2});
  }
  std::string prettyPrint() override;
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(Expression *expr1, Expression *expr2, Expression *expr3)
      : Expression("conditional-expression") {
    this->children.insert(this->children.end(), {expr1, expr2, expr3});
  }
};

// AST nodes for statements

class Statement : public ASTNode {
private:
public:
  explicit Statement(const std::string &name, const Token *token = nullptr)
      : ASTNode(name, token) {}

  virtual std::string prettyPrintBlock(int lvl) {
    return "\n" + this->prettyPrint(lvl);
  }
  virtual std::string prettyPrintScopeIndent(int lvl) {
    return this->prettyPrintBlock(lvl) + indent(lvl - 1);
  }
  virtual std::string prettyPrintInlineIf(int lvl) {
    return this->prettyPrintBlock(lvl);
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

private:
  std::string graphWalker() override;
};

class LabelStatement : public Statement {
public:
  LabelStatement(Expression *ident, Statement *stmt)
      : Statement("labeled-statement", nullptr) {
    this->children.insert(this->children.end(), {ident, stmt});
  }
};

class CompoundStatement : public Statement {
private:
  std::vector<Statement *> block;

public:
  CompoundStatement(const Token *token, std::vector<Statement *> block)
      : Statement("compound-statement", token), block(block) {
    this->children.insert(this->children.end(), block.begin(), block.end());
  }
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
};

class ExpressionStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ExpressionStatement(const Token *token, Expression *expr = nullptr)
      : Statement("expression-statement", expr == nullptr ? nullptr : token),
        expr(expr) {
    this->children.insert(this->children.end(), {expr});
  }
  std::string prettyPrint(int lvl) override;
};

class IfElseStatement : public Statement {
private:
  Expression *expr;
  Statement *ifStat;
  Statement *elseStat;

public:
  IfElseStatement(const Token *token, Expression *expr, Statement *ifStat,
                  Statement *elseStat = nullptr)
      : Statement("selection-statement", token), expr(expr), ifStat(ifStat),
        elseStat(elseStat) {
    this->children.insert(this->children.end(), {expr, ifStat, elseStat});
  }
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
};

class WhileStatement : public Statement {
private:
  Expression *expr;
  Statement *stat;

public:
  WhileStatement(const Token *token, Expression *expr, Statement *stat)
      : Statement("iteration-statement", token), expr(expr), stat(stat) {
    this->children.insert(this->children.end(), {expr, stat});
  }
  std::string prettyPrint(int lvl) override;
};

class GotoStatement : public Statement {
public:
  explicit GotoStatement(Expression *ident)
      : Statement("jump-statement", nullptr) {
    this->children.insert(this->children.end(), {ident});
  }
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string prettyPrint(int lvl) override;
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string prettyPrint(int lvl) override;
};

class ReturnStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ReturnStatement(const Token *token, Expression *expr = nullptr)
      : Statement("jump-statement", token), expr(expr) {
    this->children.insert(this->children.end(), {expr});
  }
  std::string prettyPrint(int lvl) override;
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
