#include "../utils/utils.hpp"
#include <utility>

#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#define FRIENDS                                                                \
  friend SemanticVisitor;                                                      \
  friend GraphvizVisitor;                                                      \
  friend PrettyPrinterVisitor;

#include "../lexer/token.hpp"
#include "../utils/macros.hpp"
#include "raw_type.hpp"
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

class ASTNode;
class Expression;
class Statement;
class VariableName;
class Type;
class Declarator;
class ExternalDeclaration;
class Declaration;
class ParamDeclaration;
class Visitor;
class SemanticVisitor;
class GraphvizVisitor;
class PrettyPrinterVisitor;
class StructType;
class AbstractDeclarator;
class Number;

using DeclarationListType = std::vector<std::unique_ptr<Declaration>>;
using ExternalDeclarationListType =
    std::vector<std::unique_ptr<ExternalDeclaration>>;
using ParamDeclarationListType = std::vector<std::unique_ptr<ParamDeclaration>>;
using ExpressionListType = std::vector<std::unique_ptr<Expression>>;
using StatementListType = std::vector<std::unique_ptr<Statement>>;
using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;

// Base class for all nodes in AST.
class ASTNode {
  Token tok;

protected:
  explicit ASTNode(Token tk) : tok(std::move(tk)) {}

public:
  virtual ~ASTNode() = default;
  virtual std::string accept(Visitor *) = 0;
  Token &getTokenRef() { return tok; }
  unsigned long hash() { return (unsigned long)this; }
  virtual bool isLValue() { return false; }
};

class TranslationUnit : public ASTNode {
  FRIENDS
  ExternalDeclarationListType extern_list;

public:
  explicit TranslationUnit(const Token &tk, ExternalDeclarationListType e)
      : ASTNode(tk), extern_list(std::move(e)) {}
  std::string accept(Visitor *) override;
};

class ExternalDeclaration : public ASTNode {
protected:
  explicit ExternalDeclaration(const Token &tk) : ASTNode(tk) {}
};

class FunctionDefinition : public ExternalDeclaration {
  FRIENDS
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;
  std::unique_ptr<Statement> fn_body;

public:
  FunctionDefinition(const Token &tk, std::unique_ptr<Type> r,
                     std::unique_ptr<Declarator> n,
                     std::unique_ptr<Statement> b)
      : ExternalDeclaration(tk), return_type(std::move(r)),
        fn_name(std::move(n)), fn_body(std::move(b)) {}
  std::string accept(Visitor *) override;
};

class Declaration : public ExternalDeclaration {
protected:
  explicit Declaration(const Token &tk) : ExternalDeclaration(tk) {}
};

class FunctionDeclaration : public Declaration {
  FRIENDS
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;

public:
  FunctionDeclaration(const Token &tk, std::unique_ptr<Type> r,
                      std::unique_ptr<Declarator> n)
      : Declaration(tk), return_type(std::move(r)), fn_name(std::move(n)) {}
  std::string accept(Visitor *) override;
};

class DataDeclaration : public Declaration {
  FRIENDS
  std::unique_ptr<Type> data_type;
  std::unique_ptr<Declarator> data_name;

public:
  DataDeclaration(const Token &tk, std::unique_ptr<Type> t,
                  std::unique_ptr<Declarator> n)
      : Declaration(tk), data_type(std::move(t)), data_name(std::move(n)) {}
  std::string accept(Visitor *) override;
};

class StructDeclaration : public Declaration {
  FRIENDS
  std::unique_ptr<Type> struct_type; // TODO StructType
  std::unique_ptr<Declarator> struct_alias;

public:
  StructDeclaration(const Token &tk, std::unique_ptr<Type> t,
                    std::unique_ptr<Declarator> a = nullptr)
      : Declaration(tk), struct_type(std::move(t)), struct_alias(std::move(a)) {
  }
  std::string accept(Visitor *) override;
};

class ParamDeclaration : public Declaration {
  FRIENDS
  std::unique_ptr<Type> param_type;
  std::unique_ptr<Declarator> param_name;

public:
  ParamDeclaration(const Token &tk, std::unique_ptr<Type> t,
                   std::unique_ptr<Declarator> n = nullptr)
      : Declaration(tk), param_type(std::move(t)), param_name(std::move(n)) {}
  std::string accept(Visitor *) override;
};

class Type : public ASTNode {
protected:
  explicit Type(const Token &tk) : ASTNode(tk) {}
  virtual bool isStructType() { return false; }

public:
  virtual StructType *getStructType() { return nullptr; }
};

enum class ScalarTypeValue { VOID, CHAR, INT };

class ScalarType : public Type {
  FRIENDS
  ScalarTypeValue type_kind;

public:
  ScalarType(const Token &tk, ScalarTypeValue v) : Type(tk), type_kind(v) {}
  std::string accept(Visitor *) override;
};

class StructType : public Type {
  FRIENDS
  std::unique_ptr<VariableName> struct_name;
  ExternalDeclarationListType member_list;
  bool isStructType() override { return true; }
  bool members;

public:
  StructType(const Token &tk, std::unique_ptr<VariableName> n)
      : Type(tk), struct_name(std::move(n)), members(false) {}
  StructType(const Token &tk, std::unique_ptr<VariableName> n,
             ExternalDeclarationListType m)
      : Type(tk), struct_name(std::move(n)), member_list(std::move(m)),
        members(true) {}
  std::string accept(Visitor *) override;
  StructType *getStructType() override { return this; }
};

class Declarator : public ASTNode {
protected:
  explicit Declarator(const Token &tk) : ASTNode(tk) {}

public:
  virtual std::unique_ptr<VariableName> *getIdentifier() = 0;
  virtual AbstractDeclarator *getAbstractDeclarator() { return nullptr; };
};

class DirectDeclarator : public Declarator {
  FRIENDS
  std::unique_ptr<VariableName> identifer;
  std::unique_ptr<VariableName> *getIdentifier() override { return &identifer; }

public:
  DirectDeclarator(const Token &tk, std::unique_ptr<VariableName> i)
      : Declarator(tk), identifer(std::move(i)) {}
  std::string accept(Visitor *) override;
};

enum class AbstractDeclType { Data, Function };

class AbstractDeclarator : public Declarator {
  FRIENDS
  AbstractDeclType type_kind;
  unsigned int pointerCount = 0;
  std::unique_ptr<VariableName> *getIdentifier() override { return nullptr; }

public:
  AbstractDeclarator(const Token &tk, AbstractDeclType t, unsigned int p)
      : Declarator(tk), type_kind(t), pointerCount(p) {}
  std::string accept(Visitor *) override;
  AbstractDeclarator *getAbstractDeclarator() override { return this; };
};

class PointerDeclarator : public Declarator {
  FRIENDS
  std::unique_ptr<Declarator> identifier;
  int indirection_level;
  std::unique_ptr<VariableName> *getIdentifier() override {
    if (identifier)
      return identifier->getIdentifier();
    return nullptr;
  }

public:
  explicit PointerDeclarator(const Token &tk,
                             std::unique_ptr<Declarator> i = nullptr, int l = 1)
      : Declarator(tk), identifier(std::move(i)), indirection_level(l) {}
  std::string accept(Visitor *) override;
};

class FunctionDeclarator : public Declarator {
  FRIENDS
  std::unique_ptr<Declarator> identifier;
  ParamDeclarationListType param_list;
  std::unique_ptr<Declarator> return_ptr;

public:
  FunctionDeclarator(const Token &tk, std::unique_ptr<Declarator> i,
                     ParamDeclarationListType p,
                     std::unique_ptr<Declarator> r = nullptr)
      : Declarator(tk), identifier(std::move(i)), param_list(std::move(p)),
        return_ptr(std::move(r)) {}
  std::string accept(Visitor *) override;
  std::unique_ptr<VariableName> *getIdentifier() override {
    return identifier->getIdentifier();
  }
};

class Statement : public ASTNode {
protected:
  explicit Statement(const Token &tk) : ASTNode(tk) {}
};

class CompoundStmt : public Statement {
  FRIENDS
  ASTNodeListType block_items;

public:
  CompoundStmt(const Token &tk, ASTNodeListType block)
      : Statement(tk), block_items(std::move(block)) {}
  std::string accept(Visitor *) override;
};

class IfElse : public Statement {
  FRIENDS
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> ifStmt;
  std::unique_ptr<Statement> elseStmt;

public:
  IfElse(const Token &tk, std::unique_ptr<Expression> c,
         std::unique_ptr<Statement> i, std::unique_ptr<Statement> e = nullptr)
      : Statement(tk), condition(std::move(c)), ifStmt(std::move(i)),
        elseStmt(std::move(e)) {}
  std::string accept(Visitor *) override;
};

class Label : public Statement {
  FRIENDS
  std::unique_ptr<VariableName> label_name;
  std::unique_ptr<Statement> stmt;

public:
  Label(const Token &tk, std::unique_ptr<VariableName> e,
        std::unique_ptr<Statement> b)
      : Statement(tk), label_name(std::move(e)), stmt(std::move(b)) {}
  std::string accept(Visitor *) override;
};

class While : public Statement {
  FRIENDS
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Statement> block;

public:
  While(const Token &tk, std::unique_ptr<Expression> e,
        std::unique_ptr<Statement> b)
      : Statement(tk), predicate(std::move(e)), block(std::move(b)) {}
  std::string accept(Visitor *) override;
};

class Goto : public Statement {
  FRIENDS
  std::unique_ptr<VariableName> label_name;

public:
  Goto(const Token &tk, std::unique_ptr<VariableName> e)
      : Statement(tk), label_name(std::move(e)) {}
  std::string accept(Visitor *) override;
};

class ExpressionStmt : public Statement {
  FRIENDS
  std::unique_ptr<Expression> expr;

public:
  ExpressionStmt(const Token &tk, std::unique_ptr<Expression> e)
      : Statement(tk), expr(std::move(e)) {}
  std::string accept(Visitor *) override;
};

class Break : public Statement {
public:
  explicit Break(const Token &tk) : Statement(tk) {}
  std::string accept(Visitor *) override;
};

class Return : public Statement {
  FRIENDS
  std::unique_ptr<Expression> expr;

public:
  explicit Return(const Token &tk, std::unique_ptr<Expression> e = nullptr)
      : Statement(tk), expr(std::move(e)) {}
  std::string accept(Visitor *) override;
};

class Continue : public Statement {
public:
  explicit Continue(const Token &tk) : Statement(tk) {}
  std::string accept(Visitor *) override;
};

class Expression : public ASTNode {
protected:
  explicit Expression(const Token &tk) : ASTNode(tk) {}

public:
  virtual VariableName *getVariableName() { return nullptr; }
  virtual Number *getNumber() { return nullptr; }
};

class VariableName : public Expression {
  FRIENDS
  friend StructType;
  std::string name;

public:
  VariableName(const Token &tk, std::string n)
      : Expression(tk), name(std::move(n)) {}
  std::string accept(Visitor *) override;

  int Compare(const VariableName &d) const { return d.name == name; }
  bool operator==(const VariableName &d) const { return !Compare(d); }
  VariableName *getVariableName() override { return this; }
  bool isLValue() override { return true; }
};

class Number : public Expression {
  FRIENDS
  long num_value;

public:
  Number(const Token &tk, long v) : Expression(tk), num_value(v) {}
  std::string accept(Visitor *) override;
  Number *getNumber() override { return this; }
};

class Character : public Expression {
  FRIENDS
  char char_value;

public:
  Character(const Token &tk, char c) : Expression(tk), char_value(c) {}
  std::string accept(Visitor *) override;
};

class String : public Expression {
  FRIENDS
  std::string str_value;

public:
  String(const Token &tk, std::string v)
      : Expression(tk), str_value(std::move(v)) {}
  std::string accept(Visitor *) override;
};

enum class PostFixOpValue { DOT, ARROW };

class MemberAccessOp : public Expression {
  FRIENDS
  PostFixOpValue op_kind;
  std::unique_ptr<Expression> struct_name;
  std::unique_ptr<Expression> member_name;

public:
  MemberAccessOp(const Token &tk, PostFixOpValue o,
                 std::unique_ptr<Expression> s, std::unique_ptr<Expression> m)
      : Expression(tk), op_kind(o), struct_name(std::move(s)),
        member_name(std::move(m)) {}
  std::string accept(Visitor *) override;
  bool isLValue() override { return true; }
};

class ArraySubscriptOp : public Expression {
  FRIENDS
  std::unique_ptr<Expression> array_name;
  std::unique_ptr<Expression> index_value;

public:
  ArraySubscriptOp(const Token &tk, std::unique_ptr<Expression> a,
                   std::unique_ptr<Expression> i)
      : Expression(tk), array_name(std::move(a)), index_value(std::move(i)) {}
  std::string accept(Visitor *) override;
  bool isLValue() override { return true; }
};

// Function call
class FunctionCall : public Expression {
  FRIENDS
  std::unique_ptr<Expression> callee_name;
  ExpressionListType callee_args;

public:
  FunctionCall(const Token &tk, std::unique_ptr<Expression> n,
               ExpressionListType a)
      : Expression(tk), callee_name(std::move(n)), callee_args(std::move(a)) {}
  std::string accept(Visitor *) override;
};

enum class UnaryOpValue { ADDRESS_OF = 0, DEREFERENCE, MINUS, NOT };

class Unary : public Expression {
  FRIENDS
  UnaryOpValue op_kind;
  std::unique_ptr<Expression> operand;

public:
  Unary(const Token &tk, UnaryOpValue v, std::unique_ptr<Expression> o)
      : Expression(tk), op_kind(v), operand(std::move(o)) {}
  std::string accept(Visitor *) override;
  Number *getNumber() override { return operand->getNumber(); };
  bool isLValue() override { return true; }
};

class SizeOf : public Expression {
  FRIENDS
  std::unique_ptr<Type> type_name;
  std::unique_ptr<Expression> operand;

public:
  SizeOf(const Token &tk, std::unique_ptr<Type> n)
      : Expression(tk), type_name(std::move(n)) {}
  SizeOf(const Token &tk, std::unique_ptr<Expression> o)
      : Expression(tk), operand(std::move(o)) {}
  std::string accept(Visitor *) override;
};

enum class BinaryOpValue {
  MULTIPLY = 0,
  ADD,
  SUBTRACT,
  LESS_THAN,
  EQUAL,
  NOT_EQUAL,
  LOGICAL_AND,
  LOGICAL_OR,
};

class Binary : public Expression {
  FRIENDS
  BinaryOpValue op_kind;
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Binary(const Token &tk, BinaryOpValue v, std::unique_ptr<Expression> l,
         std::unique_ptr<Expression> r)
      : Expression(tk), op_kind(v), left_operand(std::move(l)),
        right_operand(std::move(r)) {}
  std::string accept(Visitor *) override;
};

class Ternary : public Expression {
  FRIENDS
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Expression> left_branch;
  std::unique_ptr<Expression> right_branch;

public:
  Ternary(const Token &tk, std::unique_ptr<Expression> c,
          std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
      : Expression(tk), predicate(std::move(c)), left_branch(std::move(l)),
        right_branch(std::move(r)) {}
  std::string accept(Visitor *) override;
};

class Assignment : public Expression {
  FRIENDS
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Assignment(const Token &tk, std::unique_ptr<Expression> l,
             std::unique_ptr<Expression> r)
      : Expression(tk), left_operand(std::move(l)),
        right_operand(std::move(r)) {}
  std::string accept(Visitor *) override;
};

class Visitor {
public:
  Visitor() = default;
  virtual ~Visitor() = default;
  virtual std::string visitTranslationUnit(TranslationUnit *) = 0;
  virtual std::string visitFunctionDefinition(FunctionDefinition *) = 0;
  virtual std::string visitFunctionDeclaration(FunctionDeclaration *) = 0;
  virtual std::string visitDataDeclaration(DataDeclaration *) = 0;
  virtual std::string visitStructDeclaration(StructDeclaration *) = 0;
  virtual std::string visitParamDeclaration(ParamDeclaration *) = 0;
  virtual std::string visitScalarType(ScalarType *) = 0;
  virtual std::string visitStructType(StructType *) = 0;
  virtual std::string visitDirectDeclarator(DirectDeclarator *) = 0;
  virtual std::string visitAbstractDeclarator(AbstractDeclarator *) = 0;
  virtual std::string visitPointerDeclarator(PointerDeclarator *) = 0;
  virtual std::string visitFunctionDeclarator(FunctionDeclarator *) = 0;
  virtual std::string visitCompoundStmt(CompoundStmt *) = 0;
  virtual std::string visitIfElse(IfElse *) = 0;
  virtual std::string visitLabel(Label *) = 0;
  virtual std::string visitWhile(While *) = 0;
  virtual std::string visitGoto(Goto *) = 0;
  virtual std::string visitExpressionStmt(ExpressionStmt *) = 0;
  virtual std::string visitBreak(Break *) = 0;
  virtual std::string visitReturn(Return *) = 0;
  virtual std::string visitContinue(Continue *) = 0;
  virtual std::string visitVariableName(VariableName *) = 0;
  virtual std::string visitNumber(Number *) = 0;
  virtual std::string visitCharacter(Character *) = 0;
  virtual std::string visitString(String *) = 0;
  virtual std::string visitMemberAccessOp(MemberAccessOp *) = 0;
  virtual std::string visitArraySubscriptOp(ArraySubscriptOp *) = 0;
  virtual std::string visitFunctionCall(FunctionCall *) = 0;
  virtual std::string visitUnary(Unary *) = 0;
  virtual std::string visitSizeOf(SizeOf *) = 0;
  virtual std::string visitBinary(Binary *) = 0;
  virtual std::string visitTernary(Ternary *) = 0;
  virtual std::string visitAssignment(Assignment *) = 0;
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
