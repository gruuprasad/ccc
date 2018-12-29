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
  unsigned long id;   // Graphviz related.
  std::string name;   // Text representing type of the AST node.
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
  }
  virtual std::string prettyPrintInlineElse(int lvl) {
    return this->prettyPrintInline(lvl);
  }
  virtual std::string prettyPrintInlineIf(int lvl) {
    return this->prettyPrintInline(lvl);
  }
  virtual std::string checkSemantic() { return "?"; };
  virtual std::string toCode() { return "?"; };
  std::string toGraph();
 
  virtual ~ASTNode() {
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
      : ASTNode("translationUnit", token, {child}) {}

  TranslationUnit(const Token *token, std::vector<ASTNode *> children = {})
      : ASTNode("translationUnit", token, std::move(children)) {}

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
  Expression(std::string name, const Token *token = nullptr,
                      std::vector<ASTNode *> children = {})
      : ASTNode(std::move(name), token, std::move(children)) {}

private:
  std::string graphWalker() override;
};

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(const Token *token)
      : Expression("primary expression", token, {}){};
  PrimaryExpression(std::string name, const Token *token)
      : Expression(std::move(name), token, {}){};
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
  std::string prettyPrint() override;
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

protected:
  std::string indent(int n);
private:
  std::string graphWalker() override; 
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
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineElse(int lvl) override; 
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(const Token *token, Expression *expr = nullptr)
      : Statement("expression-statement", expr == nullptr ? nullptr : token,
                  {expr}) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineElse(int lvl) override;
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(const Token *token, Expression *expr, Statement *stmt1,
                  Statement *stmt2 = nullptr)
      : Statement("selection-statement", token, {expr, stmt1, stmt2}) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
};

class WhileStatement : public Statement {
public:
  WhileStatement(const Token *token, Expression *expr, Statement *stmt)
      : Statement("iteration-statement", token, {expr, stmt}) {}
  std::string prettyPrint(int lvl) override;
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
  std::string prettyPrint(int lvl) override; 
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token *token)
      : Statement("jump-statement", token) {}
  std::string prettyPrint(int lvl) override; 
};

class ReturnStatement : public Statement {
public:
  explicit ReturnStatement(const Token *token, Expression *expr = nullptr)
      : Statement("jump-statement", token, std::vector<ASTNode *>{expr}) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineElse(int lvl) override;
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
