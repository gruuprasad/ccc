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
  std::string name; // Text representing type of the AST node, only used for gv
  Location loc;
  explicit ASTNode(std::string name, Location loc = Location(0, 0))
      : name(std::move(name)), loc(loc) {}

public:
#if GRAPHVIZ
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->graphWalker() + "}\n}\n";
  }
  Token getToken() { return Token(TokenType::GHOST, loc); }
  const std::string &getName() const { return name; }
  virtual std::string walk(ASTNode *root, std::vector<ASTNode *> children) = 0;
  virtual std::string graphWalker() {
    return walk(this, std::vector<ASTNode *>());
  }
#endif
  virtual std::string prettyPrint() { return this->prettyPrint(0); };
  virtual std::string prettyPrint(int) { return "?"; };
  bool checkSemantic() { return true; };
  template <class C> C *cast() { return dynamic_cast<C *>(this); }

  virtual ~ASTNode() = default;
  template <class C> bool instanceof () { return (dynamic_cast<C *>(this)); }
};

class TranslationUnit : public ASTNode { // TODO
private:
  std::vector<ASTNode *> children;

public:
  TranslationUnit(const Token &token, ASTNode *child)
      : ASTNode("translationUnit", token.getLocation()), children({child}) {}

  TranslationUnit(const Token &token, std::vector<ASTNode *> children)
      : ASTNode("translationUnit", token.getLocation()),
        children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;
  ~TranslationUnit() override {
    for (ASTNode *node : children)
      delete (node);
  }
};

class Declaration : public ASTNode { // TODO
public:
  Declaration(ASTNode *ident, TypeSpecifier type)
      : ASTNode("declaration"), ident(ident), type(type) {}
#if GRAPHVIZ
  std::string graphWalker() override;
#endif

private:
  ASTNode *ident;
  TypeSpecifier type;
};

class InitDeclaration : Declaration { // TODO
public:
  InitDeclaration(ASTNode *ident, TypeSpecifier type)
      : Declaration(ident, type) {}
};

// AST nodes for expression
class Expression : public ASTNode {
public:
  explicit Expression(std::string name, Location loc = Location(0, 0))
      : ASTNode(std::move(name), loc) {}
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::vector<ASTNode *> children) override;
#endif
};

class PrimaryExpression : public Expression {
private:
  std::string extra;

public:
  PrimaryExpression(std::string name, const Token &token)
      : Expression(std::move(name), token.getLocation()),
        extra(token.getExtra()){};
  std::string prettyPrint(int) override { return extra; }
#if GRAPHVIZ
  std::string graphWalker() override;
#endif
};

class Identifier : public PrimaryExpression {
public:
  explicit Identifier(const Token &token = Token())
      : PrimaryExpression("identifier", token) {}
};

class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token &token = Token())
      : PrimaryExpression("string-literal", token) {}
};

class Constant : public PrimaryExpression {
public:
  explicit Constant(const Token &token = Token())
      : PrimaryExpression("constant", token) {}
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
      : Expression("additive-expression", token.getLocation()), leftExpr(expr1),
        rightExpr(expr2), op(token.name()) {}
  std::string prettyPrint() override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, {leftExpr, rightExpr});
  }
#endif
  ~BinaryExpression() override {
    delete (leftExpr);
    delete (rightExpr);
  }
};

class ConditionalExpression : public Expression { // TODO
private:
  Expression *condExpr;
  Expression *ifExpr;
  Expression *elseExpr;

public:
  ConditionalExpression(Expression *expr1, Expression *expr2, Expression *expr3)
      : Expression("conditional-expression"), condExpr(expr1), ifExpr(expr2),
        elseExpr(expr3) {}
  ~ConditionalExpression() override {
    delete (condExpr);
    delete (ifExpr);
    delete (elseExpr);
  }
};

// AST nodes for statements

class Statement : public ASTNode {
private:
public:
  explicit Statement(const std::string &name, Location loc = Location(0, 0))
      : ASTNode(name, loc) {}

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
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::vector<ASTNode *> children) override;
  std::string graphWalker() override {
    return walk(this, std::vector<ASTNode *>());
  }
#endif
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

class LabeledStatement : public Statement {
private:
  Expression *expr;
  Statement *stat;

public:
  explicit LabeledStatement(Expression *expr, Statement *stat = nullptr)
      : Statement("labeled-statement"), expr(expr), stat(stat) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, {expr, stat}); }
#endif
  std::string prettyPrint(int lvl) override;
  ~LabeledStatement() override {
    delete (expr);
    delete (stat);
  }
};

class CompoundStatement : public Statement {
private:
  std::vector<Statement *> block;

public:
  CompoundStatement(const Token &token, std::vector<Statement *> block)
      : Statement("compound-statement", token.getLocation()),
        block(std::move(block)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, std::vector<ASTNode *>(block.begin(), block.end()));
  }
#endif
  ~CompoundStatement() override {
    for (Statement *stat : block)
      delete (stat);
  }
};

class ExpressionStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ExpressionStatement(const Token &token, Expression *expr = nullptr)
      : Statement("expression-statement", token.getLocation()), expr(expr) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, {expr}); }
#endif
  std::string prettyPrint(int lvl) override;
  ~ExpressionStatement() override { delete (expr); }
};

class IfElseStatement : public Statement {
private:
  Expression *expr;
  Statement *ifStat;
  Statement *elseStat;

public:
  IfElseStatement(const Token &token, Expression *expr, Statement *ifStat,
                  Statement *elseStat = nullptr)
      : Statement("selection-statement", token.getLocation()), expr(expr),
        ifStat(ifStat), elseStat(elseStat) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintBlock(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, {expr, ifStat, elseStat});
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
      : Statement("iteration-statement", token.getLocation()), expr(expr),
        stat(stat) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, {expr, stat}); }
#endif
  ~WhileStatement() override {
    delete (expr);
    delete (stat);
  }
  std::string prettyPrint(int lvl) override;
};

class GotoStatement : public Statement {
private:
  Expression *expr;

public:
  explicit GotoStatement(Expression *expr)
      : Statement("jump-statement"), expr(expr) {}
  std::string prettyPrint(int lvl) override;
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, {expr}); }
#endif
  ~GotoStatement() override { delete (expr); }
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token &token)
      : Statement("jump-statement", token.getLocation()) {}
  std::string prettyPrint(int lvl) override;
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token &token)
      : Statement("jump-statement", token.getLocation()) {}
  std::string prettyPrint(int lvl) override;
};

class ReturnStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ReturnStatement(const Token &token, Expression *expr = nullptr)
      : Statement("jump-statement", token.getLocation()), expr(expr) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, {expr}); }
#endif
  std::string prettyPrint(int lvl) override;
  ~ReturnStatement() override { delete (expr); }
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
