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
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>

#pragma GCC diagnostic pop
namespace ccc {

class CodegenVisitor : public Visitor<void> {
  llvm::LLVMContext ctx;
  llvm::Module mod;
  llvm::IRBuilder<> builder, allocBuilder;
  llvm::Function *parent;

  std::vector<llvm::BasicBlock *> breaks;
  std::vector<llvm::BasicBlock *> continues;
  std::unordered_map<std::string, llvm::BasicBlock *> labels;
  std::unordered_map<std::string, llvm::BasicBlock *> ulabels;
  std::unordered_map<std::string, llvm::Value *> declarations;

  llvm::Value *rec_val;
  llvm::Value *tmp;

public:
  CodegenVisitor() : mod("test", ctx), builder(ctx), allocBuilder(ctx){};
  ~CodegenVisitor() override = default;

  void dump() {
    for (const auto &p : ulabels) {
      builder.SetInsertPoint(p.second);
      builder.CreateBr(labels[p.first]);
    }

    mod.dump();
    std::error_code EC;
    llvm::raw_fd_ostream stream("test.ll", EC,
                                llvm::sys::fs::OpenFlags::F_Text);
    mod.print(stream, nullptr);
  }

  void visitTranslationUnit(TranslationUnit *v) override {
    for (const auto &e : v->extern_list)
      e->accept(this);
  }

  void visitFunctionDefinition(FunctionDefinition *v) override {

    v->fn_name->accept(this);

    v->fn_body->accept(this);

    if (builder.GetInsertBlock()->getTerminator() == nullptr) {
      llvm::Type *CurFuncReturnType = builder.getCurrentFunctionReturnType();
      if (CurFuncReturnType->isVoidTy()) {
        builder.CreateRetVoid();
      } else {
        builder.CreateRet(llvm::Constant::getNullValue(CurFuncReturnType));
      }
    }
  }

  void visitFunctionDeclaration(FunctionDeclaration *v) override { (void)v; }

  void visitDataDeclaration(DataDeclaration *v) override { (void)v; }

  void visitStructDeclaration(StructDeclaration *v) override { (void)v; }

  void visitParamDeclaration(ParamDeclaration *v) override { (void)v; }

  void visitScalarType(ScalarType *v) override { (void)v; }

  void visitAbstractType(AbstractType *v) override { (void)v; }

  void visitStructType(StructType *v) override { (void)v; }

  void visitDirectDeclarator(DirectDeclarator *v) override { (void)v; }

  void visitAbstractDeclarator(AbstractDeclarator *v) override { (void)v; }

  void visitPointerDeclarator(PointerDeclarator *v) override { (void)v; }

  void visitFunctionDeclarator(FunctionDeclarator *v) override {
    llvm::Type *FuncMaxReturnType = builder.getInt32Ty();

    std::vector<llvm::Type *> FuncMaxParamTypes;

    FuncMaxParamTypes.push_back(builder.getInt32Ty());

    llvm::FunctionType *FuncMaxType =
        llvm::FunctionType::get(FuncMaxReturnType, FuncMaxParamTypes, false);

    parent =
        llvm::Function::Create(FuncMaxType, llvm::GlobalValue::ExternalLinkage,
                               (*v->identifier->getIdentifier())->name, &mod);

    llvm::BasicBlock *FuncMaxEntryBB =
        llvm::BasicBlock::Create(ctx, "entry", parent, nullptr);
    builder.SetInsertPoint(FuncMaxEntryBB);
    allocBuilder.SetInsertPoint(FuncMaxEntryBB);
    int i = 0;
    for (auto &a : parent->args()) {
      a.setName((*v->param_list[i]->param_name->getIdentifier())->name);
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *ArgVarAPtr = allocBuilder.CreateAlloca(a.getType());
      builder.CreateStore(&a, ArgVarAPtr);
      declarations[(*v->param_list[i]->param_name->getIdentifier())
                       ->getUIdentifier()] = ArgVarAPtr;
      i++;
    };
  }

  void visitCompoundStmt(CompoundStmt *v) override {
    //    llvm::BasicBlock *block =
    //        llvm::BasicBlock::Create(ctx, "compound", parent, nullptr);
    //    builder.CreateBr(block);
    //    builder.SetInsertPoint(block);
    for (const auto &s : v->block_items)
      s->accept(this);
  }

  void visitIfElse(IfElse *v) override {
    llvm::BasicBlock *IfHeaderBlock =
        llvm::BasicBlock::Create(ctx, "if-header", parent, nullptr);
    llvm::BasicBlock *IfConsequenceBlock =
        llvm::BasicBlock::Create(ctx, "if-consequence", parent, nullptr);
    llvm::BasicBlock *IfAlternativeBlock =
        llvm::BasicBlock::Create(ctx, "if-alternative", parent, nullptr);
    llvm::BasicBlock *IfEndBlock =
        llvm::BasicBlock::Create(ctx, "if-end", parent, nullptr);
    builder.CreateBr(IfHeaderBlock);
    builder.SetInsertPoint(IfHeaderBlock);
    v->condition->accept(this);
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0));
    builder.CreateCondBr(c, IfConsequenceBlock, IfAlternativeBlock);
    builder.SetInsertPoint(IfConsequenceBlock);
    v->ifStmt->accept(this);
    builder.CreateBr(IfEndBlock);
    builder.SetInsertPoint(IfAlternativeBlock);
    v->elseStmt->accept(this);
    builder.CreateBr(IfEndBlock);
    builder.SetInsertPoint(IfEndBlock);
  }

  void visitLabel(Label *v) override {
    llvm::BasicBlock *l =
        llvm::BasicBlock::Create(ctx, v->label_name->name, parent, nullptr);
    labels[v->label_name->name] = l;
    builder.CreateBr(l);
    builder.SetInsertPoint(l);
    v->stmt->accept(this);
  }

  void visitWhile(While *v) override {
    llvm::BasicBlock *whileHeaderBlock =
        llvm::BasicBlock::Create(ctx, "while-header", parent, nullptr);
    llvm::BasicBlock *whileBodyBlock =
        llvm::BasicBlock::Create(ctx, "while-body", parent, nullptr);
    llvm::BasicBlock *whileEndBlock =
        llvm::BasicBlock::Create(ctx, "while-end", parent, nullptr);
    continues.push_back(whileHeaderBlock);
    breaks.push_back(whileEndBlock);
    builder.CreateBr(whileHeaderBlock);
    builder.SetInsertPoint(whileHeaderBlock);
    v->predicate->accept(this);
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0));
    builder.CreateCondBr(c, whileBodyBlock, whileEndBlock);
    builder.SetInsertPoint(whileBodyBlock);
    v->block->accept(this);
    builder.CreateBr(whileHeaderBlock);
    builder.SetInsertPoint(whileEndBlock);
    continues.pop_back();
    breaks.pop_back();
  }

  void visitGoto(Goto *v) override {
    (void)v;
    llvm::BasicBlock *b =
        llvm::BasicBlock::Create(ctx, v->label_name->name, parent, nullptr);
    builder.CreateBr(b);
    ulabels[v->label_name->name] = b;
  }

  void visitExpressionStmt(ExpressionStmt *v) override { (void)v; }

  void visitBreak(Break *) override { builder.CreateBr(breaks.back()); }

  void visitReturn(Return *v) override { (void)v; }

  void visitContinue(Continue *) override {
    builder.CreateBr(continues.back());
  }

  void visitVariableName(VariableName *v) override {
    rec_val = builder.CreateLoad(declarations[v->getUIdentifier()]);
  }

  void visitNumber(Number *v) override {
    rec_val = builder.getInt32(static_cast<uint32_t>(v->num_value));
  }

  void visitCharacter(Character *v) override { (void)v; }

  void visitString(String *v) override { (void)v; }

  void visitMemberAccessOp(MemberAccessOp *v) override { (void)v; }

  void visitArraySubscriptOp(ArraySubscriptOp *v) override { (void)v; }

  void visitFunctionCall(FunctionCall *v) override { (void)v; }

  void visitUnary(Unary *v) override { (void)v; }

  void visitSizeOf(SizeOf *v) override { (void)v; }

  void visitBinary(Binary *v) override {
    v->left_operand->accept(this);
    auto lhs = rec_val;
    v->right_operand->accept(this);
    rec_val = builder.CreateICmpSLT(lhs, rec_val);
    rec_val = builder.CreateZExt(rec_val, builder.getInt32Ty());
  }

  void visitTernary(Ternary *v) override { (void)v; }

  void visitAssignment(Assignment *v) override { (void)v; }
};

} // namespace ccc

#endif // C4_CODEGEN_VISITOR_HPP
