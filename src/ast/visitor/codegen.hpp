#ifndef C4_CODEGEN_VISITOR_HPP
#define C4_CODEGEN_VISITOR_HPP

#include "../ast_node.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#pragma GCC diagnostic pop
namespace ccc {

class CodegenVisitor : public Visitor<llvm::Value *> {

public:
  CodegenVisitor() = default;
  ~CodegenVisitor() override = default;

  llvm::Value *visitTranslationUnit(TranslationUnit *v) override {
    return v->accept(this);
  }

  llvm::Value *visitFunctionDefinition(FunctionDefinition *v) override {
    return v->accept(this);
  }

  llvm::Value *visitFunctionDeclaration(FunctionDeclaration *v) override {
    return v->accept(this);
  }

  llvm::Value *visitDataDeclaration(DataDeclaration *v) override {
    return v->accept(this);
  }

  llvm::Value *visitStructDeclaration(StructDeclaration *v) override {
    return v->accept(this);
  }

  llvm::Value *visitParamDeclaration(ParamDeclaration *v) override {
    return v->accept(this);
  }

  llvm::Value *visitScalarType(ScalarType *v) override {
    return v->accept(this);
  }

  llvm::Value *visitAbstractType(AbstractType *v) override {
    return v->accept(this);
  }

  llvm::Value *visitStructType(StructType *v) override {
    return v->accept(this);
  }

  llvm::Value *visitDirectDeclarator(DirectDeclarator *v) override {
    return v->accept(this);
  }

  llvm::Value *visitAbstractDeclarator(AbstractDeclarator *v) override {
    return v->accept(this);
  }

  llvm::Value *visitPointerDeclarator(PointerDeclarator *v) override {
    return v->accept(this);
  }

  llvm::Value *visitFunctionDeclarator(FunctionDeclarator *v) override {
    return v->accept(this);
  }

  llvm::Value *visitCompoundStmt(CompoundStmt *v) override {
    return v->accept(this);
  }

  llvm::Value *visitIfElse(IfElse *v) override { return v->accept(this); }

  llvm::Value *visitLabel(Label *v) override { return v->accept(this); }

  llvm::Value *visitWhile(While *v) override { return v->accept(this); }

  llvm::Value *visitGoto(Goto *v) override { return v->accept(this); }

  llvm::Value *visitExpressionStmt(ExpressionStmt *v) override {
    return v->accept(this);
  }

  llvm::Value *visitBreak(Break *v) override { return v->accept(this); }

  llvm::Value *visitReturn(Return *v) override { return v->accept(this); }

  llvm::Value *visitContinue(Continue *v) override { return v->accept(this); }

  llvm::Value *visitVariableName(VariableName *v) override {
    return v->accept(this);
  }

  llvm::Value *visitNumber(Number *v) override { return v->accept(this); }

  llvm::Value *visitCharacter(Character *v) override { return v->accept(this); }

  llvm::Value *visitString(String *v) override { return v->accept(this); }

  llvm::Value *visitMemberAccessOp(MemberAccessOp *v) override {
    return v->accept(this);
  }

  llvm::Value *visitArraySubscriptOp(ArraySubscriptOp *v) override {
    return v->accept(this);
  }

  llvm::Value *visitFunctionCall(FunctionCall *v) override {
    return v->accept(this);
  }

  llvm::Value *visitUnary(Unary *v) override { return v->accept(this); }

  llvm::Value *visitSizeOf(SizeOf *v) override { return v->accept(this); }

  llvm::Value *visitBinary(Binary *v) override { return v->accept(this); }

  llvm::Value *visitTernary(Ternary *v) override { return v->accept(this); }

  llvm::Value *visitAssignment(Assignment *v) override {
    return v->accept(this);
  }
};

} // namespace ccc

#endif // C4_CODEGEN_VISITOR_HPP
