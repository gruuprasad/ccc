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

// Base class for all nodes in AST. 
class ASTNode {
  Token tok;
protected:
  ASTNode(const Token & tk) : tok (tk) {}

public:
  virtual ~ASTNode() = default;
};

class TranslationUnit : public ASTNode {
  ExternalDeclarationListType extern_list;

public:
  explicit TranslationUnit(const Token & tk, ExternalDeclarationListType e)
    : ASTNode(tk), extern_list(std::move(e)) {}
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
};

class ParamDeclaration : public Declaration {
  std::unique_ptr<Type> param_type;
  std::unique_ptr<Declarator> param_name;

public:
  ParamDeclaration(const Token & tk,
                   std::unique_ptr<Type> t,
                   std::unique_ptr<Declarator> n)
    : Declaration(tk),
      param_type(std::move(t)), param_name(std::move(n)) {}
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
};

class PointerDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  int indirection_level;

public:
  PointerDeclarator(const Token & tk, std::unique_ptr<Declarator> i, int l = 1)
    : Declarator(tk), identifer(std::move(i)), indirection_level(l) {}
};

class FunctionDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  ParamDeclarationListType param_list;
  bool pointerIgnored = true;

public:
  FunctionDeclarator(const Token & tk,
                     std::unique_ptr<Declarator> i,
                     ParamDeclarationListType p,
                     bool ptri = true)
    : Declarator(tk),
      identifer(std::move(i)), param_list(std::move(p)), pointerIgnored(ptri) {}
};

class Statement : public ASTNode {
public:
  Statement(const Token & tk)
    : ASTNode(tk) {}
};

class CompoundStmt : public Statement {
public:
  CompoundStmt(const Token & tk)
    : Statement(tk) {}
};
/*
class IfElse: public Statement {

};

class Label: public Statement {

};

class While : public Statement {

};

class Goto : public Statement {

};

class ExpressionStmt : public Statement {

};

class Break : public Statement {

};

class Return : public Statement {

};

class Continue : public Statement {

};
*/
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
};

class Number : public Expression {
  int num_value;

public:
  Number(const Token & tk, int v)
    : Expression(tk), num_value(v) {}
};

class Character : public Expression {
  char char_value;

public:
  Character(const Token & tk, char c)
    : Expression(tk), char_value(c) {}
};

class String : public Expression {
  std::string str_value;

public:
  String(const Token & tk, std::string v)
    : Expression(tk), str_value(std::move(v)) {}
};

enum class PostFixOpValue { DOT, ARROW };

class MemberAccessOp : public Expression {
  PostFixOpValue op_kind;
  std::unique_ptr<Expression> struct_name;
  std::unique_ptr<Expression> member_name;

public:
  MemberAccessOp(const Token & tk, std::unique_ptr<Expression> s, std::unique_ptr<Expression> m)
    : Expression(tk),
      struct_name(std::move(s)), member_name(std::move(m)) {}
};

class ArraySubscriptOp : public Expression {
  std::unique_ptr<Expression> array_name;
  std::unique_ptr<Expression> index_value;

public:
  ArraySubscriptOp(const Token & tk, std::unique_ptr<Expression> a, std::unique_ptr<Expression> i)
    : Expression(tk),
      array_name(std::move(a)), index_value(std::move(i)) {}
};

// Function call
class FunctionCall : public Expression {
  std::unique_ptr<Expression> callee_name;
  ExpressionListType callee_args;

public:
  FunctionCall(const Token & tk, std::unique_ptr<Expression> n, ExpressionListType a)
    : Expression(tk),
      callee_name(std::move(n)), callee_args(std::move(a)) {}
};

enum class UnaryOpValue { ADDRESS_OF, DEREFERENCE, MINUS, NOT };

class Unary : public Expression {
  UnaryOpValue op_kind;
  std::unique_ptr<Expression> operand;

public:
  Unary(const Token & tk, UnaryOpValue v, std::unique_ptr<Expression> o)
    : Expression(tk), 
      op_kind(v), operand(std::move(o)) {}
};

class SizeOf : public Expression {
  std::unique_ptr<Type> type_name;
  std::unique_ptr<Expression> operand;

public:
  SizeOf(const Token & tk, std::unique_ptr<Type> n, std::unique_ptr<Expression> o)
    : Expression(tk),
      type_name(std::move(n)), operand(std::move(o)) {}
};

enum class BinaryOpValue { MULTIPLY, ADD, SUBTRACT, LESS_THAN, EQUAL, NOT_EQUAL, LOGICAL_AND, LOGICAL_OR, ASSIGN };

class Binary : public Expression {
  BinaryOpValue op_kind;
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Binary(const Token & tk, BinaryOpValue v, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk), 
      op_kind(v), left_operand(std::move(l)), right_operand(std::move(r)) {}
};

class Ternary : public Expression {
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Expression> left_branch;
  std::unique_ptr<Expression> right_branch;

public:
  Ternary(const Token & tk, std::unique_ptr<Expression> c, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk), 
      predicate(std::move(c)), left_branch(std::move(l)), right_branch(std::move(r)) {}
};

class Assignment : public Expression {
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Assignment(const Token & tk, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : Expression(tk),
      left_operand(std::move(l)), right_operand(std::move(r)) {}
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
