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
using ExternalDeclarationListType =
    std::vector<std::unique_ptr<ExternalDeclaration>>;
using ParamDeclarationListType = std::vector<std::unique_ptr<ParamDeclaration>>;
using StatementListType = std::vector<std::unique_ptr<Statement>>;

// Base class for all nodes in AST.
class ASTNode {
protected:
  Token tok;
  explicit ASTNode(Token tk) : tok(std::move(tk)) {}
  std::string indent(int lvl) { return std::string(lvl, TAB); }
  std::string error;

public:
  virtual ~ASTNode() = default;
  virtual std::string prettyPrint(int lvl) = 0;
};

class TranslationUnit : public ASTNode {
  ExternalDeclarationListType extern_list;

public:
  explicit TranslationUnit(const Token &tk, ExternalDeclarationListType e)
      : ASTNode(tk), extern_list(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
};

class ExternalDeclaration : public ASTNode {

public:
  explicit ExternalDeclaration(const Token &tk) : ASTNode(tk) {}
};

class FunctionDefinition : public ExternalDeclaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;
  std::unique_ptr<Statement> fn_body;

public:
  FunctionDefinition(const Token &tk, std::unique_ptr<Type> r,
                     std::unique_ptr<Declarator> n,
                     std::unique_ptr<Statement> b)
      : ExternalDeclaration(tk), return_type(std::move(r)),
        fn_name(std::move(n)), fn_body(std::move(b)) {}

  std::string prettyPrint(int lvl) override;
};

class Declaration : public ExternalDeclaration {
public:
  explicit Declaration(const Token &tk) : ExternalDeclaration(tk) {}
};

class FunctionDeclaration : public Declaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;

public:
  FunctionDeclaration(const Token &tk,
                      std::unique_ptr<Type> r, // XXX Change it to param type
                      std::unique_ptr<Declarator> n)
      : Declaration(tk), return_type(std::move(r)), fn_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class DataDeclaration : public Declaration {
  std::unique_ptr<Type> data_type;
  std::unique_ptr<Declarator> data_name;

public:
  DataDeclaration(const Token &tk, std::unique_ptr<Type> t,
                  std::unique_ptr<Declarator> n)
      : Declaration(tk), data_type(std::move(t)), data_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class StructDeclaration : public Declaration {
  std::unique_ptr<Type> struct_type;
  std::unique_ptr<Declarator> struct_alias;

public:
  StructDeclaration(const Token &tk, std::unique_ptr<Type> t,
                    std::unique_ptr<Declarator> a)
      : Declaration(tk), struct_type(std::move(t)), struct_alias(std::move(a)) {
  }

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
  explicit Type(const Token &tk) : ASTNode(tk) {}
  virtual bool isStructType() = 0;
};

enum class ScalarTypeValue { VOID, CHAR, INT };

class ScalarType : public Type {
  ScalarTypeValue type_kind;

public:
  ScalarType(const Token &tk, ScalarTypeValue v) : Type(tk), type_kind(v) {}
  bool isStructType() override { return false; }

  std::string prettyPrint(int lvl) override;
};

class StructType : public Type {
  std::string struct_name;
  ExternalDeclarationListType member_list;

public:
  StructType(const Token &tk, std::string n)
      : Type(tk), struct_name(std::move(n)) {}
  StructType(const Token &tk, std::string n, ExternalDeclarationListType m)
      : Type(tk), struct_name(std::move(n)), member_list(std::move(m)) {}
  bool isStructType() override { return true; }

  std::string prettyPrint(int lvl) override;
};

class Declarator : public ASTNode {

public:
  explicit Declarator(const Token &tk) : ASTNode(tk) {}
};

class DirectDeclarator : public Declarator {
  std::unique_ptr<VariableName> identifer;

public:
  DirectDeclarator(const Token &tk, std::unique_ptr<VariableName> i)
      : Declarator(tk), identifer(std::move(i)) {}

  std::string prettyPrint(int lvl) override;
};

class PointerDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  int indirection_level;

public:
  PointerDeclarator(const Token &tk, std::unique_ptr<Declarator> i, int l = 1)
      : Declarator(tk), identifer(std::move(i)), indirection_level(l) {}

  std::string prettyPrint(int lvl) override;
};

class FunctionDeclarator : public Declarator {
  std::unique_ptr<Declarator> identifer;
  ParamDeclarationListType param_list;
  bool pointerIgnored = true; // XXX what does this switch?

public:
  FunctionDeclarator(const Token &tk, std::unique_ptr<Declarator> i,
                     ParamDeclarationListType p, bool ptri = true)
      : Declarator(tk), identifer(std::move(i)), param_list(std::move(p)),
        pointerIgnored(ptri) {}

  std::string prettyPrint(int lvl) override;
};

class Statement : public ASTNode {
public:
  explicit Statement(const Token &tk) : ASTNode(tk) {}
};

class CompoundStmt : public Statement {
  StatementListType block;

public:
  CompoundStmt(const Token &tk, StatementListType block)
      : Statement(tk), block(std::move(block)) {}

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
//
// class Label: public Statement {
//
//};
//
// class While : public Statement {
//
//};
//
// class Goto : public Statement {
//
//};

class ExpressionStmt : public Statement {
  std::unique_ptr<Expression> expression;

public:
  explicit ExpressionStmt(const Token &tk, std::unique_ptr<Expression> expr)
      : Statement(tk), expression(std::move(expr)) {}

  std::string prettyPrint(int lvl) override;
};

// class Break : public Statement {
//
//};
//
// class Return : public Statement {
// std::string prettyPrint(int lvl) override;
//};
//
// class Continue : public Statement {
//
//};

class Expression : public ASTNode {

public:
  explicit Expression(const Token &tk) : ASTNode(tk) {}
};

class VariableName : public Expression {
  std::string name;

public:
  VariableName(const Token &tk, std::string n)
      : Expression(tk), name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
};

class Number : public Expression {
  int num_value;

public:
  Number(const Token &tk, int v) : Expression(tk), num_value(v) {}

  std::string prettyPrint(int lvl) override;
};

class Character : public Expression {
  char char_value;

public:
  Character(const Token &tk, char c) : Expression(tk), char_value(c) {}

  std::string prettyPrint(int lvl) override;
};

class String : public Expression {
  std::string str_value;

public:
  String(const Token &tk, std::string v)
      : Expression(tk), str_value(std::move(v)) {}

  std::string prettyPrint(int lvl) override;
};

// class Unary : public Expression {
//
//};

class Binary : public Expression {
  std::unique_ptr<Expression> left_expression;
  std::unique_ptr<Expression> right_expression;

public:
  Binary(const Token &tk, std::unique_ptr<Expression> l,
         std::unique_ptr<Expression> r)
      : Expression(tk), left_expression(std::move(l)),
        right_expression(std::move(r)) {
    tok.name(); // XXX removing this results in a cmake ld error
  }

  std::string prettyPrint(int lvl) override;
};

// class PostFix : public Expression {
//
//};

} // namespace ccc

#endif // C4_ASTNODE_HPP
