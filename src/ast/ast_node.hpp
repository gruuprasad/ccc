#include <utility>

#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include "scope_handler.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ccc {

enum class TypeSpec { VOID, CHAR, INT, STRUCT, POINTER, FUNCTION };

class ASTNode {
private:
  Location location;

protected:
  explicit ASTNode(Location location = Location(0, 0)) : location(location) {}

public:
  virtual std::string prettyPrint(int) { return "?"; };
  const Location getLocation() { return location; }
  template <class C> C *cast() { return dynamic_cast<C *>(this); }
  virtual bool checkType() { return true; }
  virtual ~ASTNode() = default;
  virtual bool isTypeExpression() { return false; }
#if GRAPHVIZ
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
           this->graphWalker() + "}\n}\n";
  }
  //  Token getToken() { return Token(TokenType::GHOST, location); }
  virtual std::string walk(ASTNode *root, std::string name,
                           std::vector<ASTNode *> children) = 0;
  virtual std::string graphWalker() = 0;
#endif
};

// AST nodes for expression
class Expression : public ASTNode {
protected:
  explicit Expression(Location loc = Location(0, 0)) : ASTNode(loc) {}

public:
  virtual std::string getIdentifier() { return ""; }
#if GRAPHVIZ
  std::string walk(ASTNode *root, std::string name,
                   std::vector<ASTNode *> children) override {
    std::stringstream ss;
    ss << (unsigned long)this << "[label=<" << name;
    ss << "<br/><font point-size='10'>" << root->getLocation() << "</font>";
    ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
    for (ASTNode *child : children) {
      if (child) {
        ss << child->graphWalker() << (unsigned long)this << "--"
           << (unsigned long)child << std::endl;
      }
    }
    return ss.str();
  }
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
                   std::vector<ASTNode *>) override {
    std::stringstream ss;
    ss << (unsigned long)this << "[label=<" << name << " " << this->extra;
    ss << "<br/><font point-size='10'>" << root->getLocation() << "</font>";
    ss << "> shape=diamond style=filled fillcolor=lightyellow];\n";
    return ss.str();
  }
#endif
};

class IdentifierExpression : public PrimaryExpression {
public:
  explicit IdentifierExpression(const Token &token)
      : PrimaryExpression(token) {}
  std::string getIdentifier() { return this->prettyPrint(0); }

#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, "identifier", {}); }
#endif
};

class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token &token) : PrimaryExpression(token) {}
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "string-literal", {});
  }
#endif
};

class ConstantExpression : public PrimaryExpression {
public:
  explicit ConstantExpression(const Token &token) : PrimaryExpression(token) {}
#if GRAPHVIZ
  std::string graphWalker() override { return walk(this, "constant", {}); }
#endif
};

class CallExpression : public Expression {
private:
  Expression *call;
  std::vector<Expression *> args;

public:
  explicit CallExpression(const Token &token, Expression *call,
                          std::vector<Expression *> args)
      : Expression(token.getLocation()), call(call), args(std::move(args)) {}
  std::string prettyPrint(int) override;
  ~CallExpression() override {
    delete (call);
    for (Expression *arg : args)
      delete (arg);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "call-expression", {});
  }
#endif
};

class UnaryExpression : public Expression {
private:
  Expression *expr;
  std::string op;

public:
  UnaryExpression(const Token &token, Expression *expr)
      : Expression(token.getLocation()), expr(expr), op(token.name()) {}
  std::string prettyPrint(int) override {
    return "(" + op + expr->prettyPrint(0) + ")";
  }
  ~UnaryExpression() override { delete (expr); }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "uanry-expression", {expr});
  }
#endif
};

class PostfixExpression : public Expression {
private:
  Expression *expr;
  Expression *post;
  std::string op;

public:
  PostfixExpression(const Token &token, Expression *expr, Expression *post)
      : Expression(token.getLocation()), expr(expr), post(post),
        op(token.name()) {}
  std::string prettyPrint(int) override {
    return "(" + expr->prettyPrint(0) + op + post->prettyPrint(0) + ")";
  }
  ~PostfixExpression() override {
    delete (expr);
    delete (post);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "postfix-expression", {expr});
  }
#endif
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
  std::string prettyPrint(int) override;

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

class ConditionalExpression : public Expression {
private:
  Expression *condExpr;
  Expression *ifExpr;
  Expression *elseExpr;

public:
  ConditionalExpression(const Token &token, Expression *expr1,
                        Expression *expr2, Expression *expr3)
      : Expression(token.getLocation()), condExpr(expr1), ifExpr(expr2),
        elseExpr(expr3) {}
  std::string prettyPrint(int) override {
    std::stringstream ss;
    ss << "(" << condExpr->prettyPrint(0) << " ? " << ifExpr->prettyPrint(0)
       << " : " << elseExpr->prettyPrint(0) << ")";
    return ss.str();
  }
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
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "type-expression", {});
  }
#endif
};

class PointerTypeExpression : public TypeExpression {
private:
  Expression *expr;

public:
  explicit PointerTypeExpression(TypeExpression *expr)
      : TypeExpression(TypeSpec::POINTER), expr(expr) {}
  explicit PointerTypeExpression(IdentifierExpression *expr)
      : TypeExpression(TypeSpec::POINTER), expr(expr) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() { return expr->getIdentifier(); }
  ~PointerTypeExpression() override { delete (expr); }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "type-expression", {});
  }
#endif
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
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "type-expression", {});
  }
#endif
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
  std::string getIdentifier() { return expr->getIdentifier(); }
  ~FunctionTypeExpression() override {
    delete expr;
    for (TypeExpression *arg : args)
      delete arg;
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "type-expression", {});
  }
#endif
};

class SizeOfExpression : public Expression {
private:
  Expression *expr;

public:
  explicit SizeOfExpression(const Token &token, Expression *expr)
      : Expression(token.getLocation()), expr(expr) {}
  std::string prettyPrint(int) override {
    std::stringstream ss;
    if (expr->isTypeExpression())
      ss << "(sizeof(" << expr->prettyPrint(0) << "))";
    else
      ss << "(sizeof " << expr->prettyPrint(0) << ")";
    return ss.str();
  }
  ~SizeOfExpression() override { delete expr; }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "sizeof-expression", {});
  }
#endif
};

void printScopes(
    std::vector<std::unordered_map<std::string, TypeExpression *>> *scopes) {
  std::cout << std::endl;
  std::string pre = "";
  for (std::unordered_map<std::string, TypeExpression *> scope : *scopes) {
    for (const auto &kv : scope) {
      std::cout << pre << kv.first << " : " << kv.second->prettyPrint(0)
                << std::endl;
    }
    pre += "\t";
  }
}

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

  virtual bool
  checkType(std::vector<std::unordered_map<std::string, TypeExpression *>> *) {
    return true;
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
                   std::vector<ASTNode *> children) override {
    std::stringstream ss;
    ss << (unsigned long)root << "[label=<" << name;
    ss << "<br/><font point-size='10'>" << root->getLocation() << "</font>";
    ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
    for (ASTNode *child : children) {
      if (child) {
        ss << "subgraph cluster_" << (unsigned long)child << "{\n"
           << child->graphWalker() << "}\n";
        ss << (unsigned long)root << "--" << (unsigned long)child << ";\n";
      }
    }
    return ss.str();
  }
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
  std::string prettyPrintBlock(int lvl);

public:
  CompoundStatement(const Token &token, std::vector<Statement *> block)
      : Statement(token.getLocation()), block(std::move(block)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
  std::string prettyPrintStruct(int lvl);
  bool checkType(std::vector<std::unordered_map<std::string, TypeExpression *>>
                     *scopes) override {
    scopes->push_back(
        std::unordered_map<std::string, TypeExpression *>(block.size() / .75));
    for (Statement *stat : block) {
      if (!stat->checkType(scopes))
        return false;
    }
    scopes->pop_back();
    return true;
  }
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
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
  ~IfElseStatement() override {
    delete (expr);
    delete (ifStat);
    delete (elseStat);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    if (elseStat)
      return walk(this, "ifelse-statement", {expr, ifStat, elseStat});
    else
      return walk(this, "if-statement", {expr, ifStat, elseStat});
  }
#endif
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
    return walk(this, "while-statement", {expr, stat});
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
    return walk(this, "goto-statement", {expr});
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
    return walk(this, "break-statement", {});
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
    return walk(this, "continue-statement", {});
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
    return walk(this, "return-statement", {expr});
  }
#endif
};

class DeclarationStatement : public Statement {
private:
  TypeExpression *type;
  CompoundStatement *body;
  std::string identifier;

public:
  DeclarationStatement(const Token &token, TypeExpression *type,
                       CompoundStatement *body = nullptr)
      : Statement(token.getLocation()), type(type), body(body),
        identifier(type->getIdentifier()) {}
  std::string prettyPrint(int lvl) override;
  bool checkType(std::vector<std::unordered_map<std::string, TypeExpression *>>
                     *scopes) override {
    scopes->back()[type->getIdentifier()] = type;
    printScopes(scopes);
    return true;
  }
  ~DeclarationStatement() override {
    delete (type);
    delete (body);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    std::stringstream ss;
    ss << (unsigned long)this << "[label=\""
       << "declaration"
       << "\" shape=box style=filled fillcolor=lightsalmon];\n";
    return ss.str();
  }
#endif
};

class StructStatement : public Statement {
private:
  IdentifierExpression *name;
  CompoundStatement *body;
  IdentifierExpression *alias;

public:
  StructStatement(const Token &token, IdentifierExpression *name,
                  CompoundStatement *body, IdentifierExpression *alias)
      : Statement(token.getLocation()), name(name), body(body), alias(alias) {}
  std::string prettyPrint(int lvl) override;
  ~StructStatement() override {
    delete (name);
    delete (alias);
    delete (body);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    return walk(this, "struct-statement", {name, body, alias});
  }
#endif
};

class TranslationUnit : public ASTNode {
private:
  std::vector<Statement *> children;

public:
  explicit TranslationUnit(
      std::vector<Statement *> children = std::vector<Statement *>())
      : ASTNode(), children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;

  bool checkType() override {
    std::vector<std::unordered_map<std::string, TypeExpression *>> scopes = {
        std::unordered_map<std::string, TypeExpression *>(children.size() /
                                                          .75)};
    for (Statement *child : children) {
      if (!child->checkType(&scopes))
        return false;
    }
    return true;
  }

  ~TranslationUnit() override {
    for (Statement *child : children)
      delete (child);
  }
#if GRAPHVIZ
  std::string graphWalker() override {
    std::vector<ASTNode *> nodes;
    for (Statement *child : children)
      nodes.push_back(child);
    return walk(this, "", nodes);
  }
  std::string walk(ASTNode *, std::string,
                   std::vector<ASTNode *> children) override {
    std::stringstream ss;
    for (ASTNode *child : children)
      ss << child->graphWalker();
    return ss.str();
  }
#endif
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
