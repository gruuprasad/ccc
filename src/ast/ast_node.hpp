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

class ASTNode;
class TranslationUnit;
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
class FunctionDeclaration;
class AbstractDeclarator;
class IfElse;
class Label;
class While;
class Goto;
class ExpressionStmt;
class Break;
class Return;
class Continue;
class Number;
class Character;
class MemberAccessOp;
class String;
class ArraySubscriptOp;
class FunctionCall;
class Unary;
class SizeOf;
class Binary;
class Binary;
class Ternary;
class Assignment;

class Visitor {
public:
  Visitor() = default;
  virtual ~Visitor() = default;
  virtual void visitTranslationUnit(TranslationUnit *v) = 0;
  virtual void visitFunctionDefinition(FunctionDefinition *v) = 0;
  virtual void visitFunctionDeclaration(FunctionDeclaration *v) = 0;
  virtual void visitDataDeclaration(DataDeclaration *v) = 0;
  virtual void visitStructDeclaration(StructDeclaration *v) = 0;
  virtual void visitParamDeclaration(ParamDeclaration *v) = 0;
  virtual void visitScalarType(ScalarType *v) = 0;
  virtual void visitStructType(StructType *v) = 0;
  virtual void visitDirectDeclarator(DirectDeclarator *v) = 0;
  virtual void visitAbstractDeclarator(AbstractDeclarator *v) = 0;
  virtual void visitPointerDeclarator(PointerDeclarator *v) = 0;
  virtual void visitFunctionDeclarator(FunctionDeclarator *v) = 0;
  virtual void visitCompoundStmt(CompoundStmt *v) = 0;
  virtual void visitIfElse(IfElse *v) = 0;
  virtual void visitLabel(Label *v) = 0;
  virtual void visitWhile(While *v) = 0;
  virtual void visitGoto(Goto *v) = 0;
  virtual void visitExpressionStmt(ExpressionStmt *v) = 0;
  virtual void visitBreak(Break *v) = 0;
  virtual void visitReturn(Return *v) = 0;
  virtual void visitContinue(Continue *v) = 0;
  virtual void visitVariableName(VariableName *v) = 0;
  virtual void visitNumber(Number *v) = 0;
  virtual void visitCharacter(Character *v) = 0;
  virtual void visitString(String *v) = 0;
  virtual void visitMemberAccessOp(MemberAccessOp *v) = 0;
  virtual void visitArraySubscriptOp(ArraySubscriptOp *v) = 0;
  virtual void visitFunctionCall(FunctionCall *v) = 0;
  virtual void visitUnary(Unary *v) = 0;
  virtual void visitSizeOf(SizeOf *v) = 0;
  virtual void visitBinary(Binary *v) = 0;
  virtual void visitTernary(Ternary *v) = 0;
  virtual void visitAssignment(Assignment *v) = 0;
};

using DeclarationListType = std::vector<std::unique_ptr<Declaration>>;
using ExternalDeclarationListType =
    std::vector<std::unique_ptr<ExternalDeclaration>>;
using ParamDeclarationListType = std::vector<std::unique_ptr<ParamDeclaration>>;
using ExpressionListType = std::vector<std::unique_ptr<Expression>>;
using StatementListType = std::vector<std::unique_ptr<Statement>>;
using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;
// Base class for all nodes in AST.
class ASTNode {
public:
  Token tok;

protected:
  std::string error;
  explicit ASTNode(Token tk) : tok(std::move(tk)) {}
  std::string indent(int lvl) { return std::string(lvl, TAB); }

public:
  virtual ~ASTNode() = default;
  virtual std::string prettyPrint(int) = 0;
  virtual void accept(Visitor *v) = 0;
  Token &getTokenRef() { return tok; }

// Graphviz block
#if GRAPHVIZ
  std::string toGraph();
  virtual std::string graphviz() = 0;
#endif
};

class TranslationUnit : public ASTNode {
public:
  ExternalDeclarationListType extern_list;

public:
  explicit TranslationUnit(const Token &tk, ExternalDeclarationListType e)
      : ASTNode(tk), extern_list(std::move(e)) {}

  std::string prettyPrint(int lvl) override;

  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class ExternalDeclaration : public ASTNode {
public:
public:
  explicit ExternalDeclaration(const Token &tk) : ASTNode(tk) {}
};

class FunctionDefinition : public ExternalDeclaration {
public:
public:
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
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Declaration : public ExternalDeclaration {
public:
public:
  explicit Declaration(const Token &tk) : ExternalDeclaration(tk) {}
};

class FunctionDeclaration : public Declaration {
public:
public:
  std::unique_ptr<Type> return_type;
  std::unique_ptr<Declarator> fn_name;

public:
  FunctionDeclaration(const Token &tk, std::unique_ptr<Type> r,
                      std::unique_ptr<Declarator> n)
      : Declaration(tk), return_type(std::move(r)), fn_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class DataDeclaration : public Declaration {
public:
  std::unique_ptr<Type> data_type;
  std::unique_ptr<Declarator> data_name;

public:
  DataDeclaration(const Token &tk, std::unique_ptr<Type> t,
                  std::unique_ptr<Declarator> n)
      : Declaration(tk), data_type(std::move(t)), data_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class StructDeclaration : public Declaration {
public:
  std::unique_ptr<Type> struct_type;
  std::unique_ptr<Declarator> struct_alias;

public:
  StructDeclaration(const Token &tk, std::unique_ptr<Type> t,
                    std::unique_ptr<Declarator> a = nullptr)
      : Declaration(tk), struct_type(std::move(t)), struct_alias(std::move(a)) {
  }

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class ParamDeclaration : public Declaration {
public:
  std::unique_ptr<Type> param_type;
  std::unique_ptr<Declarator> param_name;

public:
  ParamDeclaration(const Token &tk, std::unique_ptr<Type> t,
                   std::unique_ptr<Declarator> n = nullptr)
      : Declaration(tk), param_type(std::move(t)), param_name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Type : public ASTNode {
public:
public:
  explicit Type(const Token &tk) : ASTNode(tk) {}
  virtual bool isStructType() = 0;
};

enum class ScalarTypeValue { VOID, CHAR, INT };

class ScalarType : public Type {
public:
  ScalarTypeValue type_kind;

public:
  ScalarType(const Token &tk, ScalarTypeValue v) : Type(tk), type_kind(v) {}
  bool isStructType() override { return false; }

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class StructType : public Type {
public:
  std::string struct_name;
  ExternalDeclarationListType member_list;

public:
  StructType(const Token &tk, std::string n)
      : Type(tk), struct_name(std::move(n)) {}
  StructType(const Token &tk, std::string n, ExternalDeclarationListType m)
      : Type(tk), struct_name(std::move(n)), member_list(std::move(m)) {}
  bool isStructType() override { return true; }

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Declarator : public ASTNode {
public:
public:
  explicit Declarator(const Token &tk) : ASTNode(tk) {}
};

class DirectDeclarator : public Declarator {
public:
  std::unique_ptr<VariableName> identifer;

public:
  DirectDeclarator(const Token &tk, std::unique_ptr<VariableName> i)
      : Declarator(tk), identifer(std::move(i)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

enum class AbstractDeclType { Data, Function };

class AbstractDeclarator : public Declarator {
public:
  AbstractDeclType type_kind;
  unsigned int pointerCount = 0;

public:
  AbstractDeclarator(const Token &tk, AbstractDeclType t, unsigned int p)
      : Declarator(tk), type_kind(t), pointerCount(p) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class PointerDeclarator : public Declarator {
public:
  std::unique_ptr<Declarator> identifer;
  int indirection_level;

public:
  explicit PointerDeclarator(const Token &tk,
                             std::unique_ptr<Declarator> i = nullptr, int l = 1)
      : Declarator(tk), identifer(std::move(i)), indirection_level(l) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class FunctionDeclarator : public Declarator {
public:
  std::unique_ptr<Declarator> identifer;
  ParamDeclarationListType param_list;
  std::unique_ptr<Declarator> return_ptr;

public:
  FunctionDeclarator(const Token &tk, std::unique_ptr<Declarator> i,
                     ParamDeclarationListType p,
                     std::unique_ptr<Declarator> r = nullptr)
      : Declarator(tk), identifer(std::move(i)), param_list(std::move(p)),
        return_ptr(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Statement : public ASTNode {
public:
public:
  explicit Statement(const Token &tk) : ASTNode(tk) {}
  virtual std::string prettyPrintInline(int lvl) {
    return "\n" + this->prettyPrint(lvl);
  }
  virtual std::string prettyPrintScopeIndent(int lvl) {
    return this->prettyPrintInline(lvl) + indent(lvl - 1);
  }
  virtual std::string prettyPrintInlineIf(int lvl) {
    return this->prettyPrintInline(lvl);
  }
};

class CompoundStmt : public Statement {
public:
  ASTNodeListType block_items;
  std::string prettyPrintBlock(int lvl);

public:
  CompoundStmt(const Token &tk, ASTNodeListType block)
      : Statement(tk), block_items(std::move(block)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintScopeIndent(int lvl) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class IfElse : public Statement {
public:
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> ifStmt;
  std::unique_ptr<Statement> elseStmt;

public:
  IfElse(const Token &tk, std::unique_ptr<Expression> c,
         std::unique_ptr<Statement> i, std::unique_ptr<Statement> e = nullptr)
      : Statement(tk), condition(std::move(c)), ifStmt(std::move(i)),
        elseStmt(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;
  std::string prettyPrintInline(int lvl) override;
  std::string prettyPrintInlineIf(int lvl) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Label : public Statement {
public:
  std::unique_ptr<Expression> label_name;
  std::unique_ptr<Statement> stmt;

public:
  Label(const Token &tk, std::unique_ptr<Expression> e,
        std::unique_ptr<Statement> b)
      : Statement(tk), label_name(std::move(e)), stmt(std::move(b)) {}
  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class While : public Statement {
public:
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Statement> block;

public:
  While(const Token &tk, std::unique_ptr<Expression> e,
        std::unique_ptr<Statement> b)
      : Statement(tk), predicate(std::move(e)), block(std::move(b)) {}
  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Goto : public Statement {
public:
  std::unique_ptr<Expression> label_name;

public:
  Goto(const Token &tk, std::unique_ptr<Expression> e)
      : Statement(tk), label_name(std::move(e)) {}
  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class ExpressionStmt : public Statement {
public:
  std::unique_ptr<Expression> expr;

public:
  ExpressionStmt(const Token &tk, std::unique_ptr<Expression> e)
      : Statement(tk), expr(std::move(e)) {}
  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Break : public Statement {
public:
public:
  explicit Break(const Token &tk) : Statement(tk) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Return : public Statement {
public:
  std::unique_ptr<Expression> expr;

public:
  explicit Return(const Token &tk, std::unique_ptr<Expression> e = nullptr)
      : Statement(tk), expr(std::move(e)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Continue : public Statement {
public:
public:
  explicit Continue(const Token &tk) : Statement(tk) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Expression : public ASTNode {
public:
public:
  explicit Expression(const Token &tk) : ASTNode(tk) {}
};

class VariableName : public Expression {
public:
  std::string name;

public:
  VariableName(const Token &tk, std::string n)
      : Expression(tk), name(std::move(n)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Number : public Expression {
public:
  int num_value;

public:
  Number(const Token &tk, int v) : Expression(tk), num_value(v) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Character : public Expression {
public:
  char char_value;

public:
  Character(const Token &tk, char c) : Expression(tk), char_value(c) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class String : public Expression {
public:
  std::string str_value;

public:
  String(const Token &tk, std::string v)
      : Expression(tk), str_value(std::move(v)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

enum class PostFixOpValue { DOT, ARROW };

class MemberAccessOp : public Expression {
public:
  PostFixOpValue op_kind;
  std::unique_ptr<Expression> struct_name;
  std::unique_ptr<Expression> member_name;

public:
  MemberAccessOp(const Token &tk, PostFixOpValue o,
                 std::unique_ptr<Expression> s, std::unique_ptr<Expression> m)
      : Expression(tk), op_kind(o), struct_name(std::move(s)),
        member_name(std::move(m)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class ArraySubscriptOp : public Expression {
public:
  std::unique_ptr<Expression> array_name;
  std::unique_ptr<Expression> index_value;

public:
  ArraySubscriptOp(const Token &tk, std::unique_ptr<Expression> a,
                   std::unique_ptr<Expression> i)
      : Expression(tk), array_name(std::move(a)), index_value(std::move(i)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

// Function call
class FunctionCall : public Expression {
public:
  std::unique_ptr<Expression> callee_name;
  ExpressionListType callee_args;

public:
  FunctionCall(const Token &tk, std::unique_ptr<Expression> n,
               ExpressionListType a)
      : Expression(tk), callee_name(std::move(n)), callee_args(std::move(a)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

enum class UnaryOpValue { ADDRESS_OF = 0, DEREFERENCE, MINUS, NOT };

class Unary : public Expression {
public:
  UnaryOpValue op_kind;
  std::unique_ptr<Expression> operand;

public:
  Unary(const Token &tk, UnaryOpValue v, std::unique_ptr<Expression> o)
      : Expression(tk), op_kind(v), operand(std::move(o)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class SizeOf : public Expression {
public:
  std::unique_ptr<Type> type_name;
  std::unique_ptr<Expression> operand;

public:
  SizeOf(const Token &tk, std::unique_ptr<Type> n)
      : Expression(tk), type_name(std::move(n)) {}
  SizeOf(const Token &tk, std::unique_ptr<Expression> o)
      : Expression(tk), operand(std::move(o)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
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
public:
  BinaryOpValue op_kind;
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Binary(const Token &tk, BinaryOpValue v, std::unique_ptr<Expression> l,
         std::unique_ptr<Expression> r)
      : Expression(tk), op_kind(v), left_operand(std::move(l)),
        right_operand(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Ternary : public Expression {
public:
  std::unique_ptr<Expression> predicate;
  std::unique_ptr<Expression> left_branch;
  std::unique_ptr<Expression> right_branch;

public:
  Ternary(const Token &tk, std::unique_ptr<Expression> c,
          std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
      : Expression(tk), predicate(std::move(c)), left_branch(std::move(l)),
        right_branch(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

class Assignment : public Expression {
public:
  std::unique_ptr<Expression> left_operand;
  std::unique_ptr<Expression> right_operand;

public:
  Assignment(const Token &tk, std::unique_ptr<Expression> l,
             std::unique_ptr<Expression> r)
      : Expression(tk), left_operand(std::move(l)),
        right_operand(std::move(r)) {}

  std::string prettyPrint(int lvl) override;
  void accept(Visitor *v) override;

// Graphviz block
#if GRAPHVIZ
  std::string graphviz() override;
#endif
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
