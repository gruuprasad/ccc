#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include "../utils/macros.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
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
  std::unique_ptr<Expression> call;
  std::vector<std::unique_ptr<Expression>> args;

public:
  explicit CallExpression(const Token &token, std::unique_ptr<Expression> call,
                          std::vector<std::unique_ptr<Expression>> args)
      : Expression(token), call(std::move(call)), args(std::move(args)) {}
  std::string prettyPrint(int) override;
};

class UnaryExpression : public Expression {
private:
  std::unique_ptr<Expression> expr;
  std::string op;

public:
  UnaryExpression(const Token &token, std::unique_ptr<Expression> expr)
      : Expression(token), expr(std::move(expr)), op(token.name()) {}

  bool
  nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;

  std::string prettyPrint(int) override;   
};

class PostfixExpression : public Expression {
private:
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Expression> post;
  std::string op;

public:
  PostfixExpression(const Token &token, std::unique_ptr<Expression> expr,
                    std::unique_ptr<Expression> post)
      : Expression(token), expr(std::move(expr)), post(std::move(post)),
        op(token.name()) {}
  std::string prettyPrint(int) override;
};

class BinaryExpression : public Expression {
private:
  std::unique_ptr<Expression> leftExpr;
  std::unique_ptr<Expression> rightExpr;
  std::string op;

public:
  BinaryExpression(const Token &token, std::unique_ptr<Expression> expr1,
                   std::unique_ptr<Expression> expr2)
      : Expression(token), leftExpr(std::move(expr1)),
        rightExpr(std::move(expr2)), op(token.name()) {}

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  std::string prettyPrint(int) override;
};

class ConditionalExpression : public Expression {
private:
  std::unique_ptr<Expression> condExpr;
  std::unique_ptr<Expression> ifExpr;
  std::unique_ptr<Expression> elseExpr;

public:
  ConditionalExpression(const Token &token, std::unique_ptr<Expression> expr1,
                        std::unique_ptr<Expression> expr2,
                        std::unique_ptr<Expression> expr3)
      : Expression(token), condExpr(std::move(expr1)), ifExpr(std::move(expr2)),
        elseExpr(std::move(expr3)) {}
  std::string prettyPrint(int) override; 
};

class TypeExpression : public Expression {
private:
  TypeSpec baseType;
  std::unique_ptr<Expression> expr;

public:
  explicit TypeExpression(TypeSpec baseType,
                          std::unique_ptr<TypeExpression> expr = nullptr)
      : Expression(), baseType(baseType), expr(std::move(expr)) {}
  explicit TypeExpression(TypeSpec baseType,
                          std::unique_ptr<IdentifierExpression> expr)
      : Expression(), baseType(baseType), expr(std::move(expr)) {}
  std::string prettyPrint(int) override;
  bool isTypeExpression() override { return true; }
  std::string getIdentifier() override {
    if (expr) {
      return expr->getIdentifier();
    } else
      return "";
  }
};

class PointerTypeExpression : public TypeExpression {
private:
  std::unique_ptr<Expression> expr; // TODO lvl of pointer

public:
  explicit PointerTypeExpression(std::unique_ptr<TypeExpression> expr)
      : TypeExpression(TypeSpec::POINTER), expr(std::move(expr)) {}
  explicit PointerTypeExpression(std::unique_ptr<IdentifierExpression> expr)
      : TypeExpression(TypeSpec::POINTER), expr(std::move(expr)) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
};

class StructTypeExpression : public TypeExpression {
private:
  std::unique_ptr<TypeExpression> expr;
  std::unique_ptr<IdentifierExpression> iden;

public:
  explicit StructTypeExpression(std::unique_ptr<IdentifierExpression> iden,
                                std::unique_ptr<TypeExpression> expr = nullptr)
      : TypeExpression(TypeSpec::STRUCT), expr(std::move(expr)),
        iden(std::move(iden)) {}
  std::string prettyPrint(int) override;
  bool isTypeExpression() override { return true; }
};

class FunctionTypeExpression : public TypeExpression {
private:
  std::unique_ptr<Expression> expr;
  std::vector<std::unique_ptr<TypeExpression>> args;

public:
  explicit FunctionTypeExpression(
      std::unique_ptr<TypeExpression> expr,
      std::vector<std::unique_ptr<TypeExpression>> args)
      : TypeExpression(TypeSpec::FUNCTION), expr(std::move(expr)),
        args(std::move(args)) {}
  explicit FunctionTypeExpression(
      std::unique_ptr<IdentifierExpression> expr,
      std::vector<std::unique_ptr<TypeExpression>> args)
      : TypeExpression(TypeSpec::FUNCTION), expr(std::move(expr)),
        args(std::move(args)) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
};

class SizeOfExpression : public Expression {
private:
  std::unique_ptr<Expression> expr;

public:
  explicit SizeOfExpression(const Token &token,
                            std::unique_ptr<Expression> expr)
      : Expression(token), expr(std::move(expr)) {}
  std::string prettyPrint(int) override; 
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

  virtual bool typeAnalysis(
      std::vector<
          std::unordered_map<std::string, std::unique_ptr<TypeExpression>>> *);

protected:
  explicit Statement(Token token = Token()) : ASTNode(std::move(token)) {}
  std::string indent(int n);
  void printTypes(std::vector<std::unordered_map<
                      std::string, std::unique_ptr<TypeExpression>>> *scopes);
};

class LabeledStatement : public Statement {
private:
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Statement> stat;

public:
  explicit LabeledStatement(std::unique_ptr<Expression> expr,
                            std::unique_ptr<Statement> stat = nullptr)
      : Statement(), expr(std::move(expr)), stat(std::move(stat)) {}
  std::string prettyPrint(int lvl) override;
};

class CompoundStatement : public Statement {
private:
  std::vector<std::unique_ptr<Statement>> block;
  std::string prettyPrintBlock(int lvl);

public:
  CompoundStatement(const Token &token,
                    std::vector<std::unique_ptr<Statement>> block)
      : Statement(token), block(std::move(block)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;
  std::string prettyPrintStruct(int lvl);

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  bool
  typeAnalysis(std::vector<
               std::unordered_map<std::string, std::unique_ptr<TypeExpression>>>
                   *scopes) override;
};

class ExpressionStatement : public Statement {
private:
  std::unique_ptr<Expression> expr;

public:
  explicit ExpressionStatement(const Token &token,
                               std::unique_ptr<Expression> expr = nullptr)
      : Statement(token), expr(std::move(expr)) {}
  std::string prettyPrint(int lvl) override;

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override; 
};

class IfElseStatement : public Statement {
private:
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Statement> ifStat;
  std::unique_ptr<Statement> elseStat;

public:
  IfElseStatement(const Token &token, std::unique_ptr<Expression> expr,
                  std::unique_ptr<Statement> ifStat,
                  std::unique_ptr<Statement> elseStat = nullptr)
      : Statement(token), expr(std::move(expr)), ifStat(std::move(ifStat)),
        elseStat(std::move(elseStat)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
};

class WhileStatement : public Statement {
private:
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Statement> stat;

public:
  WhileStatement(const Token &token, std::unique_ptr<Expression> expr,
                 std::unique_ptr<Statement> stat)
      : Statement(token), expr(std::move(expr)), stat(std::move(stat)) {}
  std::string prettyPrint(int lvl) override;
};

class GotoStatement : public Statement {
private:
  std::unique_ptr<Expression> expr;

public:
  explicit GotoStatement(std::unique_ptr<Expression> expr)
      : Statement(), expr(std::move(expr)) {}
  std::string prettyPrint(int lvl) override;
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
  std::unique_ptr<Expression> expr;

public:
  explicit ReturnStatement(const Token &token,
                           std::unique_ptr<Expression> expr = nullptr)
      : Statement(token), expr(std::move(expr)) {}
  std::string prettyPrint(int lvl) override;
};

class DeclarationStatement : public Statement {
private:
  std::unique_ptr<TypeExpression> type;
  std::unique_ptr<CompoundStatement> body;
  std::string identifier;

public:
  DeclarationStatement(const Token &token, std::unique_ptr<TypeExpression> type,
                       std::unique_ptr<CompoundStatement> body = nullptr)
      : Statement(token), type(std::move(type)), body(std::move(body)),
        identifier(type->getIdentifier()) {}
  std::string prettyPrint(int lvl) override;

  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  bool
  typeAnalysis(std::vector<
               std::unordered_map<std::string, std::unique_ptr<TypeExpression>>>
                   *types) override;
}; // namespace ccc

class StructStatement : public Statement {
private:
  std::unique_ptr<IdentifierExpression> name;
  std::unique_ptr<CompoundStatement> body; // TODO own block
  std::unique_ptr<IdentifierExpression> alias;

public:
  StructStatement(const Token &token,
                  std::unique_ptr<IdentifierExpression> name,
                  std::unique_ptr<CompoundStatement> body,
                  std::unique_ptr<IdentifierExpression> alias)
      : Statement(token), name(std::move(name)), body(std::move(body)),
        alias(std::move(alias)) {}
  std::string prettyPrint(int lvl) override;
};

class TranslationUnit : public ASTNode {
private:
  std::vector<std::unique_ptr<Statement>> children;

public:
  explicit TranslationUnit(std::vector<std::unique_ptr<Statement>> children =
                               std::vector<std::unique_ptr<Statement>>())
      : ASTNode(), children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;

  bool runAnalysis(); 
  bool nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) override;
  bool typeAnalysis(std::vector<std::unordered_map<
                        std::string, std::unique_ptr<TypeExpression>>> *types);
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
