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
  StructType(const Token & tk, std::string n, ExternalDeclarationListType m)
    : Type(tk),
      struct_name(std::move(n)), member_list(std::move(m)) {}
  bool isStructType() override { return true; }
};

enum class DeclaratorType { Base, Direct, Pointer, Function };

class Declarator : public ASTNode {
  DeclaratorType node_kind = DeclaratorType::Base;

public:
  Declarator(const Token & tk)
    : ASTNode(tk) {}
  DeclaratorType getDeclaratorType() { return node_kind; }
};

class DirectDeclarator : public Declarator {
  DeclaratorType node_kind = DeclaratorType::Direct;
  std::unique_ptr<VariableName> identifer;

public:
  DirectDeclarator(const Token & tk, std::unique_ptr<VariableName> i)
    : Declarator(tk),
      identifer(std::move(i)) {}
};

class PointerDeclarator : public Declarator {
  DeclaratorType node_kind = DeclaratorType::Pointer;
  std::unique_ptr<Declarator> identifer;

public:
  PointerDeclarator(const Token & tk, std::unique_ptr<Declarator> i)
    : Declarator(tk), identifer(std::move(i)) {}
};

class FunctionDeclarator : public Declarator {
  DeclaratorType node_kind  = DeclaratorType::Function;
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

/*
class Unary : public Expression {

};

class Binary : public Expression {

};

class PostFix : public Expression {

};
*/

} // namespace ccc

#endif // C4_ASTNODE_HPP
