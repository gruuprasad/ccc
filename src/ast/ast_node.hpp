#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include "../utils/macros.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ccc {

enum class TypeSpec { VOID, CHAR, INT, STRUCT, POINTER, FUNCTION };

class ASTNode {
protected:
  explicit ASTNode(Token token = Token()) : token(std::move(token)) {}
  Token token;

public:
  virtual bool nameAnalysis(std::vector<std::unordered_set<std::string>> *) {
    return true;
  }

  virtual std::string prettyPrint(int) { return "?"; };
  //  const Location getLocation() { return token.getLocation(); }
  //  template <class C> C *cast() { return dynamic_cast<C *>(this); }
  virtual ~ASTNode() = default;
  virtual bool isTypeExpression() { return false; }

  void printScopes(std::vector<std::unordered_set<std::string>> *scopes); 
};

// AST nodes for expression
class Expression : public ASTNode {
protected:
  explicit Expression(Token token = Token()) : ASTNode(std::move(token)) {}

public:
  virtual std::string getIdentifier() { return ""; }
};

class PrimaryExpression : public Expression {
protected:
  std::string extra;

public:
  explicit PrimaryExpression(const Token &token)
      : Expression(token), extra(token.getExtra()){};
  std::string prettyPrint(int) override;
  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *) override;
};

class IdentifierExpression : public PrimaryExpression {
public:
  explicit IdentifierExpression(const Token &token)
      : PrimaryExpression(token) {}
  std::string getIdentifier() override { return this->prettyPrint(0); }

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override; 
};

class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token &token) : PrimaryExpression(token) {}
};

class ConstantExpression : public PrimaryExpression {
public:
  explicit ConstantExpression(const Token &token) : PrimaryExpression(token) {}
};

class CallExpression : public Expression {
private:
  Expression *call;
  std::vector<Expression *> args;

public:
  explicit CallExpression(const Token &token, Expression *call,
                          std::vector<Expression *> args)
      : Expression(token), call(call), args(std::move(args)) {}
  std::string prettyPrint(int) override;
  ~CallExpression() override {
    delete (call);
    for (Expression *arg : args)
      delete (arg);
  }
};

class UnaryExpression : public Expression {
private:
  Expression *expr;
  std::string op;

public:
  UnaryExpression(const Token &token, Expression *expr)
      : Expression(token), expr(expr), op(token.name()) {}

  bool
  nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;

  std::string prettyPrint(int) override;   
  ~UnaryExpression() override { delete (expr); }
};

class PostfixExpression : public Expression {
private:
  Expression *expr;
  Expression *post;
  std::string op;

public:
  PostfixExpression(const Token &token, Expression *expr, Expression *post)
      : Expression(token), expr(expr), post(post), op(token.name()) {}
  std::string prettyPrint(int) override;
  ~PostfixExpression() override {
    delete (expr);
    delete (post);
  }
};

class BinaryExpression : public Expression {
private:
  Expression *leftExpr;
  Expression *rightExpr;
  std::string op;

public:
  BinaryExpression(const Token &token, Expression *expr1, Expression *expr2)
      : Expression(token), leftExpr(expr1), rightExpr(expr2), op(token.name()) {
  }

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;

  std::string prettyPrint(int) override;

  ~BinaryExpression() override {
    delete (leftExpr);
    delete (rightExpr);
  }
};

class ConditionalExpression : public Expression {
private:
  Expression *condExpr;
  Expression *ifExpr;
  Expression *elseExpr;

public:
  ConditionalExpression(const Token &token, Expression *expr1,
                        Expression *expr2, Expression *expr3)
      : Expression(token), condExpr(expr1), ifExpr(expr2), elseExpr(expr3) {}
  std::string prettyPrint(int) override; 
  ~ConditionalExpression() override {
    delete (condExpr);
    delete (ifExpr);
    delete (elseExpr);
  }
};

class TypeExpression : public Expression {
private:
  TypeSpec baseType;
  Expression *expr;

public:
  explicit TypeExpression(TypeSpec baseType, TypeExpression *expr = nullptr)
      : Expression(), baseType(baseType), expr(expr) {}
  explicit TypeExpression(TypeSpec baseType, IdentifierExpression *expr)
      : Expression(), baseType(baseType), expr(expr) {}
  std::string prettyPrint(int) override;
  bool isTypeExpression() override { return true; }
  std::string getIdentifier() override {
    if (expr) {
      return expr->getIdentifier();
    } else
      return "";
  }
  ~TypeExpression() override { delete expr; }
};

class PointerTypeExpression : public TypeExpression {
private:
  Expression *expr; // TODO lvl of pointer

public:
  explicit PointerTypeExpression(TypeExpression *expr)
      : TypeExpression(TypeSpec::POINTER), expr(expr) {}
  explicit PointerTypeExpression(IdentifierExpression *expr)
      : TypeExpression(TypeSpec::POINTER), expr(expr) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
  ~PointerTypeExpression() override { delete (expr); }
};

class StructTypeExpression : public TypeExpression {
private:
  TypeExpression *expr;
  IdentifierExpression *iden;

public:
  explicit StructTypeExpression(IdentifierExpression *iden,
                                TypeExpression *expr = nullptr)
      : TypeExpression(TypeSpec::STRUCT), expr(expr), iden(iden) {}
  std::string prettyPrint(int) override;
  bool isTypeExpression() override { return true; }
  ~StructTypeExpression() override {
    delete expr;
    delete iden;
  }
};

class FunctionTypeExpression : public TypeExpression {
private:
  Expression *expr;
  std::vector<TypeExpression *> args;

public:
  explicit FunctionTypeExpression(TypeExpression *expr,
                                  std::vector<TypeExpression *> args)
      : TypeExpression(TypeSpec::FUNCTION), expr(expr), args(std::move(args)) {}
  explicit FunctionTypeExpression(IdentifierExpression *expr,
                                  std::vector<TypeExpression *> args)
      : TypeExpression(TypeSpec::FUNCTION), expr(expr), args(std::move(args)) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
  ~FunctionTypeExpression() override {
    delete expr;
    for (TypeExpression *arg : args)
      delete arg;
  }
};

class SizeOfExpression : public Expression {
private:
  Expression *expr;

public:
  explicit SizeOfExpression(const Token &token, Expression *expr)
      : Expression(token), expr(expr) {}
  std::string prettyPrint(int) override; 
  ~SizeOfExpression() override { delete expr; }
};

// AST nodes for statements

class Statement : public ASTNode {
public:
  virtual std::string prettyPrintInline(int lvl) {
    return "\n" + this->prettyPrint(lvl);
  }
  virtual std::string prettyPrintScopeIndent(int lvl) {
    return this->prettyPrintInline(lvl) + indent(lvl - 1);
  }
  virtual std::string prettyPrintInlineIf(int lvl) {
    return this->prettyPrintInline(lvl);
  }

  virtual bool typeAnalysis(std::vector<std::unordered_map<std::string, TypeExpression *>> *);

protected:
  explicit Statement(Token token = Token()) : ASTNode(std::move(token)) {}
  std::string indent(int n); 
  void printTypes(std::vector<std::unordered_map<std::string, TypeExpression *>> *scopes);
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
};

class CompoundStatement : public Statement {
private:
  std::vector<Statement *> block;
  std::string prettyPrintBlock(int lvl);

public:
  CompoundStatement(const Token &token, std::vector<Statement *> block)
      : Statement(token), block(std::move(block)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
  std::string prettyPrintStruct(int lvl);

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  bool typeAnalysis(std::vector<std::unordered_map<std::string, TypeExpression *>> *scopes) override;

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
      : Statement(token), expr(expr) {}
  std::string prettyPrint(int lvl) override;

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override; 

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
      : Statement(token), expr(expr), ifStat(ifStat), elseStat(elseStat) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
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
      : Statement(token), expr(expr), stat(stat) {}
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
  explicit GotoStatement(Expression *expr) : Statement(), expr(expr) {}
  std::string prettyPrint(int lvl) override;
  ~GotoStatement() override { delete (expr); }
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token &token) : Statement(token) {}
  std::string prettyPrint(int lvl) override;
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token &token) : Statement(token) {}
  std::string prettyPrint(int lvl) override;
};

class ReturnStatement : public Statement {
private:
  Expression *expr;

public:
  explicit ReturnStatement(const Token &token, Expression *expr = nullptr)
      : Statement(token), expr(expr) {}
  std::string prettyPrint(int lvl) override;
  ~ReturnStatement() override { delete (expr); }
};

class DeclarationStatement : public Statement {
private:
  TypeExpression *type;
  CompoundStatement *body;
  std::string identifier;

public:
  DeclarationStatement(const Token &token, TypeExpression *type,
                       CompoundStatement *body = nullptr)
      : Statement(token), type(type), body(body),
        identifier(type->getIdentifier()) {}
  std::string prettyPrint(int lvl) override;

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override; 
  bool typeAnalysis(std::vector<std::unordered_map<std::string, TypeExpression *>> *types) override;

  ~DeclarationStatement() override {
    delete (type);
    delete (body);
  }
}; // namespace ccc

class StructStatement : public Statement {
private:
  IdentifierExpression *name;
  CompoundStatement *body; // TODO own block
  IdentifierExpression *alias;

public:
  StructStatement(const Token &token, IdentifierExpression *name,
                  CompoundStatement *body, IdentifierExpression *alias)
      : Statement(token), name(name), body(body), alias(alias) {}
  std::string prettyPrint(int lvl) override;
  ~StructStatement() override {
    delete (name);
    delete (alias);
    delete (body);
  }
};

class TranslationUnit : public ASTNode {
private:
  std::vector<Statement *> children;

public:
  explicit TranslationUnit(
      std::vector<Statement *> children = std::vector<Statement *>())
      : ASTNode(), children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;

  bool runAnalysis(); 
  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  bool typeAnalysis(std::vector<std::unordered_map<std::string, TypeExpression *>> *types);

  ~TranslationUnit() override {
    for (Statement *child : children)
      delete (child);
  }
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
