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

// predeclaration
class TypeDeclaration;

using Scope_list_type = std::vector<std::unordered_set<std::string>>;
using Type_list_type = std::vector<
    std::unordered_map<std::string, std::unique_ptr<TypeDeclaration>>>;

enum class TypeSpec { VOID, CHAR, INT, STRUCT, POINTER, FUNCTION };

// Base class for all nodes in AST. Methods are overriden by subclasses.
class ASTNode {
protected:
  explicit ASTNode(Token token = Token()) : token(std::move(token)) {}
  Token token;

public:
  virtual bool nameAnalysis(Scope_list_type *) { return true; }
  virtual std::string prettyPrint(int) { return "?"; };
  virtual ~ASTNode() = default;
  virtual bool isTypeDeclaration() { return false; }
  void printScopes(Scope_list_type *scopes);
};

// =============================================================================
// AST nodes for expression
// =============================================================================

// Base Class for expressions and type declarations.
class Expression : public ASTNode {
protected:
  explicit Expression(Token token = Token()) : ASTNode(std::move(token)) {}

public:
  virtual std::string getIdentifier() { return ""; }
};

// Base Class for singular expressions like constants or identifiers.
class PrimaryExpression : public Expression {
protected:
  std::string extra;

public:
  explicit PrimaryExpression(const Token &token)
      : Expression(token), extra(token.getExtra()){};
  std::string prettyPrint(int) override;
  bool nameAnalysis(Scope_list_type *) override;
};

// Class for identifiers.
class Identifier : public PrimaryExpression {
public:
  explicit Identifier(const Token &token) : PrimaryExpression(token) {}
  std::string getIdentifier() override { return this->prettyPrint(0); }

  bool nameAnalysis(Scope_list_type *scopes) override;
};

// Class for literal strings.
class StringLiteral : public PrimaryExpression {
public:
  explicit StringLiteral(const Token &token) : PrimaryExpression(token) {}
};

// Class for either numeric or alphabetic constants.
class Constant : public PrimaryExpression {
public:
  explicit Constant(const Token &token) : PrimaryExpression(token) {}
};

// Class for method calls
// Example:
//  (foo(a, b))
//  - call = foo
//  - args = [a, b]
class CallExpression : public Expression {
  std::unique_ptr<Expression> call;
  std::vector<std::unique_ptr<Expression>> args;

public:
  explicit CallExpression(const Token &token, std::unique_ptr<Expression> call,
                          std::vector<std::unique_ptr<Expression>> args)
      : Expression(token), call(std::move(call)), args(std::move(args)) {}
  std::string prettyPrint(int) override;
};

// Class for unary prefix Expressions like "(&a)";
class UnaryExpression : public Expression {
  std::unique_ptr<Expression> expr;
  std::string op;

public:
  UnaryExpression(const Token &token, std::unique_ptr<Expression> expr)
      : Expression(token), expr(std::move(expr)), op(token.name()) {}

  bool nameAnalysis(Scope_list_type *scopes) override;

  std::string prettyPrint(int) override;
};

// Member parameter "op" represents "[" (subscript op) or "(" (function call)
// or "." (member access) or "->" (member access for pointer type)
class PostfixExpression : public Expression {
  std::unique_ptr<Expression> expr;
  std::unique_ptr<Expression> post;
  std::string op;

public:
  PostfixExpression(
      const Token &token, std::unique_ptr<Expression> expr,
      std::unique_ptr<Expression> post = std::unique_ptr<Expression>())
      : Expression(token), expr(std::move(expr)), post(std::move(post)),
        op(token.name()) {}
  std::string prettyPrint(int) override;
};

class ArgumentExpressionList : public Expression {
public:
  ArgumentExpressionList() = default;
};

class AssignmentExpression : public Expression {
  std::unique_ptr<Expression> lhs;
  std::unique_ptr<Expression> rhs;

public:
  AssignmentExpression(const Token &token, std::unique_ptr<Expression> lhs,
                       std::unique_ptr<Expression> rhs)
      : Expression(token), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// Class for binary expressions like "(a+b)".
class BinaryExpression : public Expression {
  std::unique_ptr<Expression> leftExpr;
  std::unique_ptr<Expression> rightExpr;
  std::string op;

public:
  BinaryExpression(const Token &token, std::unique_ptr<Expression> expr1,
                   std::unique_ptr<Expression> expr2)
      : Expression(token), leftExpr(std::move(expr1)),
        rightExpr(std::move(expr2)), op(token.name()) {}

  bool nameAnalysis(Scope_list_type *scopes) override;
  std::string prettyPrint(int) override;
};

// Class for conditional expressions like "(a ? b : c)".
class ConditionalExpression : public Expression {
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

// Class to represent type declaration.
//  baseType : supported types (including pointer and function types)
//  expr : either an identifier or an nested type expression
// Example:
//  (int a)
//  - baseType = int
//  - expr = a of Identifier
// or
//  (int (*a))
//  - baseType = int
//  - expr = (*a) of PointerTypeDeclaration
class TypeDeclaration : public Expression {
  TypeSpec baseType;
  std::unique_ptr<Expression> expr;

public:
  explicit TypeDeclaration(TypeSpec baseType, std::unique_ptr<Expression> expr =
                                                  std::unique_ptr<Expression>())
      : baseType(baseType), expr(std::move(expr)) {}
  explicit TypeDeclaration(TypeSpec baseType, std::unique_ptr<Identifier> expr)
      : Expression(), baseType(baseType), expr(std::move(expr)) {}

  std::string prettyPrint(int) override;
  bool isTypeDeclaration() override { return true; }
  std::string getIdentifier() override {
    if (expr) {
      return expr->getIdentifier();
    } else
      return "";
  }
};

// Class to represent pointer to type declaration.
class PointerTypeDeclaration : public TypeDeclaration {
  std::unique_ptr<Expression> expr; // TODO lvl of pointer

public:
  explicit PointerTypeDeclaration(std::unique_ptr<TypeDeclaration> expr)
      : TypeDeclaration(TypeSpec::POINTER), expr(std::move(expr)) {}
  explicit PointerTypeDeclaration(std::unique_ptr<Identifier> expr)
      : TypeDeclaration(TypeSpec::POINTER), expr(std::move(expr)) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
};

// Class to represent Struct type declaration (no member declaration)
// Example:
//  (struct A a)
class StructTypeDeclaration : public TypeDeclaration {
  std::unique_ptr<TypeDeclaration> expr;
  std::unique_ptr<Identifier> iden;

public:
  explicit StructTypeDeclaration(std::unique_ptr<Identifier> iden,
                                 std::unique_ptr<TypeDeclaration> expr =
                                     std::unique_ptr<TypeDeclaration>())
      : TypeDeclaration(TypeSpec::STRUCT), expr(std::move(expr)),
        iden(std::move(iden)) {}
  std::string prettyPrint(int) override;
  bool isTypeDeclaration() override { return true; }
};

// Class to represent function prototype.
// Example:
//  int foo(int a, int b);
//  - expr = foo
//  - args = [int a, int b] list of arguments
class FunctionTypeDeclaration : public TypeDeclaration {
  std::unique_ptr<Expression> expr;
  std::vector<std::unique_ptr<TypeDeclaration>> args;

public:
  explicit FunctionTypeDeclaration(
      std::unique_ptr<TypeDeclaration> expr,
      std::vector<std::unique_ptr<TypeDeclaration>> args)
      : TypeDeclaration(TypeSpec::FUNCTION), expr(std::move(expr)),
        args(std::move(args)) {}
  explicit FunctionTypeDeclaration(
      std::unique_ptr<Identifier> expr,
      std::vector<std::unique_ptr<TypeDeclaration>> args)
      : TypeDeclaration(TypeSpec::FUNCTION), expr(std::move(expr)),
        args(std::move(args)) {}
  std::string prettyPrint(int) override;
  std::string getIdentifier() override { return expr->getIdentifier(); }
};

// Class for size operations like "(sizeof(int))"
class SizeOfExpression : public Expression {
  std::unique_ptr<Expression> expr;

public:
  explicit SizeOfExpression(const Token &token,
                            std::unique_ptr<Expression> expr)
      : Expression(token), expr(std::move(expr)) {}
  std::string prettyPrint(int) override;
};

// =============================================================================
// AST nodes for statements
// =============================================================================

// Base class for statements.
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

  virtual bool typeAnalysis(Type_list_type *);

protected:
  explicit Statement(Token token = Token()) : ASTNode(std::move(token)) {}
  std::string indent(int n);
  void printTypes(Type_list_type *scopes);
};

// Class for label and following statement.
// Example:
//  a:
//    return;
//  - label = a
//  - stat = return;
class LabeledStatement : public Statement {
  std::unique_ptr<Identifier> label;
  std::unique_ptr<Statement> stat;

public:
  explicit LabeledStatement(
      const Token &token, std::unique_ptr<Identifier> label,
      std::unique_ptr<Statement> stat = std::unique_ptr<Statement>())
      : Statement(token), label(std::move(label)), stat(std::move(stat)) {}
  std::string prettyPrint(int lvl) override;
};

// Class for block of statements enclosed within "{...}". This always opens a
// new scope.
class CompoundStatement : public Statement {
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
  bool nameAnalysis(Scope_list_type *scopes) override;
  bool typeAnalysis(Type_list_type *scopes) override;
};

// Class to wrap expression as statement like "(a+b);".
class ExpressionStatement : public Statement {
  std::unique_ptr<Expression> expr;

public:
  explicit ExpressionStatement(
      const Token &token,
      std::unique_ptr<Expression> expr = std::unique_ptr<Expression>())
      : Statement(token), expr(std::move(expr)) {}

  std::string prettyPrint(int lvl) override;
  bool nameAnalysis(Scope_list_type *scopes) override;
};

// Class for if-else syntax. This also opens new scopes for each statement.
// Example:
//  if (a<b) {
//    a++;
//    return a;
//  } else
//    a--;
//  - cond = (a < b)
//  - ifStat = {a++; return a;}
//  - elseStat = a--;
class IfElseStatement : public Statement {
  std::unique_ptr<Expression> cond;
  std::unique_ptr<Statement> ifStat;
  std::unique_ptr<Statement> elseStat;

public:
  IfElseStatement(
      const Token &token, std::unique_ptr<Expression> cond,
      std::unique_ptr<Statement> ifStat,
      std::unique_ptr<Statement> elseStat = std::unique_ptr<Statement>())
      : Statement(token), cond(std::move(cond)), ifStat(std::move(ifStat)),
        elseStat(std::move(elseStat)) {}
  std::string prettyPrint(int lvl) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;
};

// Class to provide loops.
// Example:
//  while (a<b)
//    a++;
//  - cond = (a < b)
//  - stat = a++;
class WhileStatement : public Statement {
  std::unique_ptr<Expression> cond;
  std::unique_ptr<Statement> stat;

public:
  WhileStatement(const Token &token, std::unique_ptr<Expression> cond,
                 std::unique_ptr<Statement> stat)
      : Statement(token), cond(std::move(cond)), stat(std::move(stat)) {}
  std::string prettyPrint(int lvl) override;
};

// Class as counterpart to labeled statements for jumps
class GotoStatement : public Statement {
  std::unique_ptr<Identifier> label;

public:
  explicit GotoStatement(std::unique_ptr<Identifier> label)
      : label(std::move(label)) {}
  std::string prettyPrint(int lvl) override;
};

// Class for break statements inside loops.
class BreakStatement : public Statement {
public:
  explicit BreakStatement(const Token &token) : Statement(token) {}
  std::string prettyPrint(int lvl) override;
};

// Class for continue statements inside loops.
class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(const Token &token) : Statement(token) {}
  std::string prettyPrint(int lvl) override;
};

// Class for return statements with value or not.
class ReturnStatement : public Statement {
  std::unique_ptr<Expression> expr;

public:
  explicit ReturnStatement(const Token &token,
                           std::unique_ptr<Expression> expr = nullptr)
      : Statement(token), expr(std::move(expr)) {}
  std::string prettyPrint(int lvl) override;
};

// Class wraps type declaration as statement and can provide a body for method
// declarations
class DeclarationStatement : public Statement {
  std::unique_ptr<TypeDeclaration> type;
  std::unique_ptr<CompoundStatement> body;
  std::string identifier;

public:
  DeclarationStatement(const Token &token,
                       std::unique_ptr<TypeDeclaration> type,
                       std::unique_ptr<CompoundStatement> body = nullptr)
      : Statement(token), type(std::move(type)), body(std::move(body)),
        identifier(type->getIdentifier()) {}

  std::string prettyPrint(int lvl) override;
  bool nameAnalysis(Scope_list_type *scopes) override;
  bool typeAnalysis(Type_list_type *types) override;
}; // namespace ccc

// Class for struct declarations.
// Example:
//  Struct S {int a;} t;
//  - name = S
//  - body = {int a;}
//  - alias = t
class StructStatement : public Statement {
  std::unique_ptr<Identifier> name;
  std::unique_ptr<CompoundStatement> body; // TODO own block
  std::unique_ptr<Identifier> alias;

public:
  StructStatement(const Token &token, std::unique_ptr<Identifier> name,
                  std::unique_ptr<CompoundStatement> body,
                  std::unique_ptr<Identifier> alias)
      : Statement(token), name(std::move(name)), body(std::move(body)),
        alias(std::move(alias)) {}
  std::string prettyPrint(int lvl) override;
};

// Class working as root for AST containg all external declarations.
class TranslationUnit : public ASTNode {
  std::vector<std::unique_ptr<Statement>> children;

public:
  TranslationUnit() = default;
  TranslationUnit(std::vector<std::unique_ptr<Statement>> children)
      : children(std::move(children)) {}

  std::string prettyPrint(int lvl) override;
  bool runAnalysis();
  bool nameAnalysis(Scope_list_type *scopes) override;
  bool typeAnalysis(Type_list_type *types);
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
