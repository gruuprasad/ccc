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

// Base class for all nodes in AST. 
class ASTNode {
  Token tok;
protected:
  ASTNode() = default;
  ASTNode(Token & t) : tok (t) {}

public:
  virtual ~ASTNode() = default;
};

class TranslationUnit : public ASTNode {
  std::vector<std::unique_ptr<Extern>> extern_list;

public:
  explicit TranslationUnit(std::vector<std::unique_ptr<ExternalDeclaration>> e)
    : extern_list(std::move(e)) {}
};

class ExternalDeclaration : public ASTNode {

};

class FunctionDefinition : public ExternalDeclaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<FunctionDeclarator> fn_name;
  std::unique_ptr<CompoundStmt> fn_body;

};

class Declaration : public ExternalDeclaration {

};

class FunctionDeclaration : public Declaration {
  std::unique_ptr<Type> return_type;
  std::unique_ptr<FunctionDeclarator> fn_name;
};

class DataDeclaration : public Declaration {
  std::unique_ptr<Type> data_type;
  std::unique_ptr<Declarator> data_name;
};

class StructDeclaration : public Declaration {
  std::unique_ptr<Type> struct_type;
  std::unique_ptr<Declarator> struct_alias;
};

class ParamDeclaration : public Declaration {
  std::unique_ptr<Type> param_type;
  std::unique_ptr<Declarator> param_name;
};

class Type : public ASTNode {
protected:
  bool isStruct;

public:
  virtual isStructType() = 0;
};

enum class ScalarTypeValue { VOID, CHAR, INT }; 

class ScalarType : public Type {
  ScalarTypeValue type_kind;

public:
  bool isStructType() override {
    return false;
  }
};

class StructType : public Type {
  std::string struct_name;
  std::vector<std::unique_ptr<Declaration>> member_list;

public:
  bool isStructType() override {
    return true;
  }
};

enum class DeclaratorType { Base, Direct, Pointer, Function };

class Declarator : public ASTNode {
  DeclaratorType node_kind = DeclaratorType::Base;

public:
  DeclaratorType getNodeType() {
    return node_kind;
  }
};

class DirectDeclarator : public Declarator {
  DeclaratorType node_kind = DeclaratorType::Direct;
  std::unique_ptr<VariableName> identifer;
};

class PointerDeclarator : public Declarator {
  DeclaratorType node_kind = DeclaratorType::Pointer;
  std::unique_ptr<Declarator> identifer;
};

class FunctionDeclarator : public Declarator {
  bool pointerIgnored = true;
  DeclaratorType node_kind  = DeclaratorType::Function;
  std::unique_ptr<Declarator> identifer;
  std::vector<std::unique_ptr<Declaration> param_list;
};

class Statement : public ASTNode {

};

class CompoundStmt : public Statement {

};

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

class Expression : public ASTNode {

};

class VariableName : public Expression {

};

class Number : public Expression {

};

class Character : public Expression {

};

class String : public Expression {

};

class Unary : public Expression {

};

class Binary : public Expression {

};

class PostFix : public Expression {

};

} // namespace ccc

#endif // C4_ASTNODE_HPP
