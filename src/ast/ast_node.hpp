#include <utility>

#include <utility>

#include <utility>

#include <utility>

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

// TODO add pointer and parameter type discription w/ comparision

class ASTNode {
protected:
  Location loc;
  explicit ASTNode(Location loc = Location(0, 0)) : loc(loc) {}

public:
  virtual std::string prettyPrint() { return this->prettyPrint(0); };
  virtual std::string prettyPrint(int) { return "?"; };
  bool checkSemantic() { return true; };
  template <class C> C *cast() { return dynamic_cast<C *>(this); }
  virtual ~ASTNode() = default;
  template <class C> bool instanceof () { return (dynamic_cast<C *>(this)); }
#if GRAPHVIZ
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->graphWalker() + "}\n}\n";
  }
  Token getToken() { return Token(TokenType::GHOST, loc); }
  virtual std::string walk(ASTNode *root, std::string name,
                           std::vector<ASTNode *> children) = 0;
  virtual std::string graphWalker() = 0;
#endif
};

class TranslationUnit : public ASTNode { // TODO
private:
  std::vector<ASTNode *> children;

public:
  TranslationUnit(const Token &token, ASTNode *child)
      : ASTNode(token.getLocation()), children({child}) {}

  TranslationUnit(const Token &token, std::vector<ASTNode *> children)
      : ASTNode(token.getLocation()), children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;
  ~TranslationUnit() override {
    for (ASTNode *node : children)
      delete (node);
  }
};

class Declaration : public ASTNode { // TODO
private:
  ASTNode *ident;
  TypeSpecifier type;

public:
  Declaration(ASTNode *ident, TypeSpecifier type)
      : ASTNode(), ident(ident), type(type) {}
#if GRAPHVIZ
  std::string graphWalker() override;
#endif
};

class InitDeclaration : Declaration { // TODO
public:
  InitDeclaration(ASTNode *ident, TypeSpecifier type)
      : Declaration(ident, type) {}
};

// AST nodes for expression
class Expression : public ASTNode {
protected:
  explicit Expression(Location loc = Location(0, 0)) : ASTNode(loc) {}

public:
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::string name,
                   std::vector<ASTNode *> children) override;
#endif
};

class PrimaryExpression : public Expression {
private:
  std::string extra;

public:
  explicit PrimaryExpression(const Token &token)
      : Expression(token.getLocation()), extra(token.getExtra()){};
  std::string prettyPrint(int) override { return extra; }
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::string name,
                   std::vector<ASTNode *> children) override;
#endif
};

class Identifier : public PrimaryExpression {
public:
  explicit Identifier(const Token &token = Token())
      : PrimaryExpression(token) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, "identifier", {}); }
#endif
};

class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token &token = Token())
      : PrimaryExpression(token) {}
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "string-literal", {});
  }
#endif
};

class Constant : public PrimaryExpression {
public:
  explicit Constant(const Token &token = Token()) : PrimaryExpression(token) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, "constant", {}); }
#endif
};

class FunctionCall : public Expression { // TODO
public:
  //  explicit FunctionCall(std::vector<ASTNode *> arguments)
  //      : Expression("postfix-expression") {
  //    this->children.insert(this->children.end(), arguments.begin(),
  //                          arguments.end());
  //  }
};

class SizeOfExpression : public Expression { // TODO
public:
  //  explicit SizeOfExpression(ASTNode *type) : Expression("unary-expression")
  //  {
  //    this->children.push_back(type);
  //  }
};

class BinaryExpression : public Expression {
private:
  Expression *leftExpr;
  Expression *rightExpr;
  std::string op;

public:
  BinaryExpression(const Token &token, Expression *expr1, Expression *expr2)
      : Expression(token.getLocation()), leftExpr(expr1), rightExpr(expr2),
        op(token.name()) {}
  std::string prettyPrint() override;

  ~BinaryExpression() override {
    delete (leftExpr);
    delete (rightExpr);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "binary-expression", {leftExpr, rightExpr});
  }
#endif
};

class ConditionalExpression : public Expression { // TODO
private:
  Expression *condExpr;
  Expression *ifExpr;
  Expression *elseExpr;

public:
  ConditionalExpression(Expression *expr1, Expression *expr2, Expression *expr3)
      : Expression(), condExpr(expr1), ifExpr(expr2), elseExpr(expr3) {}
  ~ConditionalExpression() override {
    delete (condExpr);
    delete (ifExpr);
    delete (elseExpr);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "conditional-expression", {});
  }
#endif
};

// AST nodes for statements

class Statement : public ASTNode {
public:
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
  explicit Statement(Location loc = Location(0, 0)) : ASTNode(loc) {}
  std::string indent(int n) {
    if (n >= 0) {
      std::stringstream ss;
      for (int i = 0; i < n; i++)
        ss << "\t";
      return ss.str();
    } else
      return "";
  }
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::string name,
                   std::vector<ASTNode *> children) override;
#endif
};

class LabeledStatement : public Statement {
private:
  Expression *expr;
  Statement *stat;

public:
  explicit LabeledStatement(Expression *expr, Statement *stat = nullptr)
      : Statement(), expr(expr), stat(stat) {}
  std::string prettyPrint(int lvl) override;
  ~LabeledStatement() override {
    delete (expr);
    delete (stat);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "labeled-statement", {expr, stat});
  }
#endif
};

class CompoundStatement : public Statement {
private:
  std::vector<Statement *> block;

public:
  CompoundStatement(const Token &token, std::vector<Statement *> block)
      : Statement(token.getLocation()), block(std::move(block)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
  ~CompoundStatement() override {
    for (Statement *stat : block)
      delete (stat);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "compound-statement",
                std::vector<ASTNode *>(block.begin(), block.end()));
  }
#endif
};

class ExpressionStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ExpressionStatement(const Token &token, Expression *expr = nullptr)
      : Statement(token.getLocation()), expr(expr) {}
  std::string prettyPrint(int lvl) override;
  ~ExpressionStatement() override { delete (expr); }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "expression-statement", {expr});
  }
#endif
};

class IfElseStatement : public Statement {
private:
  Expression *expr;
  Statement *ifStat;
  Statement *elseStat;

public:
  IfElseStatement(const Token &token, Expression *expr, Statement *ifStat,
                  Statement *elseStat = nullptr)
      : Statement(token.getLocation()), expr(expr), ifStat(ifStat),
        elseStat(elseStat) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "selection-statement", {expr, ifStat, elseStat});
  }
#endif
  ~IfElseStatement() override {
    delete (expr);
    delete (ifStat);
    delete (elseStat);
  }
};

class WhileStatement : public Statement {
private:
  Expression *expr;
  Statement *stat;

public:
  WhileStatement(const Token &token, Expression *expr, Statement *stat)
      : Statement(token.getLocation()), expr(expr), stat(stat) {}
  ~WhileStatement() override {
    delete (expr);
    delete (stat);
  }
  std::string prettyPrint(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "iteration-statement", {expr, stat});
  }
#endif
};

class GotoStatement : public Statement {
private:
  Expression *expr;

public:
  explicit GotoStatement(Expression *expr) : Statement(), expr(expr) {}
  std::string prettyPrint(int lvl) override;
  ~GotoStatement() override { delete (expr); }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "jump-statement", {expr});
  }
#endif
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token &token)
      : Statement(token.getLocation()) {}
  std::string prettyPrint(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "jump-statement", {});
  }
#endif
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token &token)
      : Statement(token.getLocation()) {}
  std::string prettyPrint(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "jump-statement", {});
  }
#endif
};

class ReturnStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ReturnStatement(const Token &token, Expression *expr = nullptr)
      : Statement(token.getLocation()), expr(expr) {}
  std::string prettyPrint(int lvl) override;
  ~ReturnStatement() override { delete (expr); }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "jump-statement", {expr});
  }
#endif
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
