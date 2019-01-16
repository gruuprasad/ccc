#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#define TAB '\t'

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

class ScalarType;
class Expression;
class CompoundStmt;
class Statement;
class VariableName;
class StructDeclaration;
class StructType;
class Type;
class Declarator;
class ExternalDeclaration;
class FunctionDefinition;
class FunctionDeclarator;
class FunctionDefinition;
class PointerDeclarator;
class DataDeclaration;
class DirectDeclarator;
class Declaration;
class ExternalDeclaration;
class ParamDeclaration;

using DeclarationListType = std::vector<std::unique_ptr<Declaration>>;
using ExternalDeclarationListType = std::vector<std::unique_ptr<ExternalDeclaration>>;
using ParamDeclarationListType = std::vector<std::unique_ptr<ParamDeclaration>>;
using ExpressionListType = std::vector<std::unique_ptr<Expression>>;
using StatementListType = std::vector<std::unique_ptr<Statement>>;

// Base class for all nodes in AST. 
class ASTNode {
  Token tok;
protected:
  explicit ASTNode(Token tk) : tok(std::move(tk)) {}
  std::string indent(int lvl) { return std::string(lvl, TAB); }
  std::string error;

public:
  virtual ~ASTNode() = default;
  virtual std::string prettyPrint(int lvl) = 0;
  Token & getTokenRef() { return tok; }
};

class TranslationUnit : public ASTNode {
  ExternalDeclarationListType extern_list;

public:
  explicit TranslationUnit(const Token & tk, ExternalDeclarationListType e)
    : ASTNode(tk), extern_list(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
};

class ExternalDeclaration : public ASTNode {

public:
  ExternalDeclaration(const Token & tk)
    : ASTNode(tk) {}
};

class FunctionDefinition : public ExternalDeclaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;
  std::unique_ptr<Statement> fn_body;

public:
  FunctionDefinition(const Token & tk,
                     std::unique_ptr<Type> r, 
                     std::unique_ptr<Declarator> n,
                     std::unique_ptr<Statement> b)
    : ExternalDeclaration(tk),
      return_type(std::move(r)), fn_name(std::move(n)), fn_body(std::move(b)) {}

  std::string prettyPrint(int lvl) override;
};

class Declaration : public ExternalDeclaration {
public:
  Declaration(const Token & tk)
    : ExternalDeclaration(tk) {}
};

class FunctionDeclaration : public Declaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;

public:
  FunctionDeclaration(const Token & tk,
                      std::unique_ptr<Type> r,
                      std::unique_ptr<Declarator> n)
    : Declaration(tk),
      return_type(std::move(r)), fn_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class DataDeclaration : public Declaration {
  std::unique_ptr<Type> data_type;
  std::unique_ptr<Declarator> data_name;

public:
  DataDeclaration(const Token & tk,
                  std::unique_ptr<Type> t,
                  std::unique_ptr<Declarator> n)
    : Declaration(tk),
      data_type(std::move(t)), data_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class StructDeclaration : public Declaration {
  std::unique_ptr<Type> struct_type;
  std::unique_ptr<Declarator> struct_alias;

public:
  StructDeclaration(const Token & tk,
                    std::unique_ptr<Type> t,
                    std::unique_ptr<Declarator> a)
    : Declaration(tk),
      struct_type(std::move(t)), struct_alias(std::move(a)) {}

  std::string prettyPrint(int lvl) override;
};

class ParamDeclaration : public Declaration {
  std::unique_ptr<Type> param_type;
  std::unique_ptr<Declarator> param_name;

public:
  ParamDeclaration(const Token &tk, std::unique_ptr<Type> t,
                   std::unique_ptr<Declarator> n = nullptr)
      : Declaration(tk), param_type(std::move(t)), param_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class Type : public ASTNode {
public:
  Type(const Token & tk)
    : ASTNode(tk) {}
  virtual bool isStructType() = 0;
};

enum class ScalarTypeValue { VOID, CHAR, INT }; 

class ScalarType : public Type {
  ScalarTypeValue type_kind;

public:
  ScalarType(const Token & tk, ScalarTypeValue v)
    : Type(tk),
      type_kind(v) {}
  bool isStructType() override { return false; }

  std::string prettyPrint(int lvl) override;
};

class StructType : public Type {
  std::string struct_name;
  ExternalDeclarationListType member_list;

public:
  StructType(const Token & tk, std::string n)
    : Type(tk),
      struct_name(std::move(n)) {}
  StructType(const Token & tk, std::string n, ExternalDeclarationListType m)
    : Type(tk),
      struct_name(std::move(n)), member_list(std::move(m)) {}
  bool isStructType() override { return true; }

  std::string prettyPrint(int lvl) override;
};

class Declarator : public ASTNode {

public:
  Declarator(const Token & tk)
    : ASTNode(tk) {}
};

class DirectDeclarator : public Declarator {
  std::unique_ptr<VariableName> identifer;

public:
  DirectDeclarator(const Token & tk, std::unique_ptr<VariableName> i)
    : Declarator(tk),
      identifer(std::move(i)) {}

  std::string prettyPrint(int lvl) override;
};

class PointerDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  int indirection_level;

public:
  PointerDeclarator(const Token & tk, std::unique_ptr<Declarator> i, int l = 1)
    : Declarator(tk), identifer(std::move(i)), indirection_level(l) {}

  std::string prettyPrint(int lvl) override;
};

class FunctionDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  ParamDeclarationListType param_list;

public:
  FunctionDeclarator(const Token & tk,
                     std::unique_ptr<Declarator> i,
                     ParamDeclarationListType p)
    : Declarator(tk),
      identifer(std::move(i)), param_list(std::move(p)) {}

  std::string prettyPrint(int lvl) override;
};

class Statement : public ASTNode {
public:
  Statement(const Token & tk)
    : ASTNode(tk) {}
  std::string indent(int n);
};

class CompoundStmt : public Statement {
  std::vector<std::unique_ptr<ASTNode>> block_items;
public:
  CompoundStmt(const Token &tk, std::vector<std::unique_ptr<ASTNode>> block)
      : Statement(tk), block_items(std::move(block)) {}

  std::string prettyPrint(int lvl) override;
};

class IfElse : public Statement {
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> ifStmt;
  std::unique_ptr<Statement> elseStmt;

public:
  IfElse(const Token &tk, std::unique_ptr<Expression> c,
         std::unique_ptr<Statement> i, std::unique_ptr<Statement> e = nullptr)
      : Statement(tk), condition(std::move(c)), ifStmt(std::move(i)),
        elseStmt(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
};

class Label: public Statement {
  std::unique_ptr<Expression> label_name;
  std::unique_ptr<Statement> stmt;

public:
  Label(const Token & tk, std::unique_ptr<Expression> e, std::unique_ptr<Statement> b)
    : Statement(tk), label_name(std::move(e)), stmt(std::move(b)) {}
  std::string prettyPrint(int lvl) override;
};

class While : public Statement {
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Statement> block;
public:
  While(const Token & tk, std::unique_ptr<Expression> e, std::unique_ptr<Statement> b)
    : Statement(tk), predicate(std::move(e)), block(std::move(b)) {}
  std::string prettyPrint(int lvl) override;
};

class Goto : public Statement {
  std::unique_ptr<Expression> label_name;
public:
  Goto(const Token & tk, std::unique_ptr<Expression> e)
    : Statement(tk), label_name(std::move(e)) {}
  std::string prettyPrint(int lvl) override;

};

class ExpressionStmt : public Statement {
  std::unique_ptr<Expression> expr;
public:
  ExpressionStmt(const Token & tk, std::unique_ptr<Expression> e)
    : Statement(tk), expr(std::move(e)) {}
  std::string prettyPrint(int lvl) override;

};

class Break : public Statement {
public:
  explicit Break(const Token & tk)
    : Statement(tk) {}

  std::string prettyPrint(int lvl) override;
};

class Return : public Statement {
  std::unique_ptr<Expression> expr;
public:
  explicit Return(const Token & tk, std::unique_ptr<Expression> e)
    : Statement(tk), expr(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
};

class Continue : public Statement {

public:
  explicit Continue(const Token & tk)
    : Statement(tk) {}

  std::string prettyPrint(int lvl) override;
};

class Expression : public ASTNode {
public:
  Expression(const Token & tk)
    : ASTNode(tk) {}
};

class VariableName : public Expression {
  std::string name;

public:
  VariableName(const Token & tk, std::string n)
    : Expression(tk), name(std::move(n)) {}
  
  std::string prettyPrint(int lvl) override;
};

class Number : public Expression {
  int num_value;

public:
  Number(const Token & tk, int v)
    : Expression(tk), num_value(v) {}
  
  std::string prettyPrint(int lvl) override;
};

class Character : public Expression {
  char char_value;

public:
  Character(const Token & tk, char c)
    : Expression(tk), char_value(c) {}
  
  std::string prettyPrint(int lvl) override;
};

class String : public Expression {
  std::string str_value;

public:
  String(const Token & tk, std::string v)
    : Expression(tk), str_value(std::move(v)) {}
  
  std::string prettyPrint(int lvl) override;
};

enum class PostFixOpValue { DOT, ARROW };

class MemberAccessOp : public Expression {
  PostFixOpValue op_kind;
  std::unique_ptr<Expression> struct_name;
  std::unique_ptr<Expression> member_name;

public:
  MemberAccessOp(const Token & tk, PostFixOpValue o, std::unique_ptr<Expression> s, std::unique_ptr<Expression> m)
    : Expression(tk),
      op_kind(o), struct_name(std::move(s)), member_name(std::move(m)) {}

  std::string prettyPrint(int lvl) override;
};

class ArraySubscriptOp : public Expression {
  std::unique_ptr<Expression> array_name;
  std::unique_ptr<Expression> index_value;

public:
  ArraySubscriptOp(const Token & tk, std::unique_ptr<Expression> a, std::unique_ptr<Expression> i)
    : Expression(tk),
      array_name(std::move(a)), index_value(std::move(i)) {}

  std::string prettyPrint(int lvl) override;
};

// Function call
class FunctionCall : public Expression {
  std::unique_ptr<Expression> callee_name;
  ExpressionListType callee_args;

public:
  FunctionCall(const Token & tk, std::unique_ptr<Expression> n, ExpressionListType a)
    : Expression(tk),
      callee_name(std::move(n)), callee_args(std::move(a)) {}

  std::string prettyPrint(int lvl) override;
};

enum class UnaryOpValue { ADDRESS_OF = 0, DEREFERENCE, MINUS, NOT };

class Unary : public Expression {
  UnaryOpValue op_kind;
  std::unique_ptr<Expression> operand;

public:
  Unary(const Token & tk, UnaryOpValue v, std::unique_ptr<Expression> o)
    : Expression(tk), 
      op_kind(v), operand(std::move(o)) {}
  
  std::string prettyPrint(int lvl) override;
};

class SizeOf : public Expression {
  std::unique_ptr<Type> type_name;
  std::unique_ptr<Expression> operand;

public:
  SizeOf(const Token & tk, std::unique_ptr<Type> n)
    : Expression(tk),
      type_name(std::move(n)) {}
  SizeOf(const Token & tk, std::unique_ptr<Expression> o)
    : Expression(tk),
      operand(std::move(o)) {}

  std::string prettyPrint(int lvl) override;
};

enum class BinaryOpValue { MULTIPLY = 0, ADD, SUBTRACT, LESS_THAN, EQUAL, NOT_EQUAL, LOGICAL_AND, LOGICAL_OR, ASSIGN };

class Binary : public Expression {
  BinaryOpValue op_kind;
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Binary(const Token & tk, BinaryOpValue v, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk), 
      op_kind(v), left_operand(std::move(l)), right_operand(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
};

class Ternary : public Expression {
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Expression> left_branch;
  std::unique_ptr<Expression> right_branch;

public:
  Ternary(const Token & tk, std::unique_ptr<Expression> c, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk), 
      predicate(std::move(c)), left_branch(std::move(l)), right_branch(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
};

class Assignment : public Expression {
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Assignment(const Token & tk, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk),
      left_operand(std::move(l)), right_operand(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
