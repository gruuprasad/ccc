#include <utility>

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

  std::string filename;

  std::vector<llvm::BasicBlock *> breaks;
  std::vector<llvm::BasicBlock *> continues;
  std::unordered_map<std::string, llvm::BasicBlock *> labels;
  std::unordered_map<std::string, llvm::BasicBlock *> ulabels;
  std::unordered_map<std::string, llvm::Value *> declarations;
  std::unordered_map<std::string, llvm::Function *> functions;

  llvm::Value *rec_val;
  llvm::Value *load;

public:
  CodegenVisitor(std::string f)
      : mod(f, ctx), builder(ctx), allocBuilder(ctx), filename(std::move(f)){};
  ~CodegenVisitor() override = default;

  void dump() { mod.dump(); }

  void compile() {
    for (const auto &p : ulabels) {
      builder.SetInsertPoint(p.second);
      builder.CreateBr(labels[p.first]);
    }
    llvm::verifyModule(mod);
    std::error_code EC;
    auto of = filename.substr(0, filename.rfind(".c")) + ".ll";
    if (of.find('/'))
      of = of.substr(of.rfind('/') + 1, of.size());
    llvm::raw_fd_ostream stream(of, EC, llvm::sys::fs::OpenFlags::F_Text);
    mod.print(stream, nullptr);
  }

  void visitTranslationUnit(TranslationUnit *v) override {
    for (const auto &e : v->extern_list)
      e->accept(this);
  }

  void visitFunctionDefinition(FunctionDefinition *v) override {
    if (!v->isFuncPtr) {
      if (functions.find(v->getUIdentifier()) != functions.end())
        parent = functions[v->getUIdentifier()];
      else {
        parent =
            llvm::Function::Create(v->getUType()->getLLVMFunctionType(builder),
                                   llvm::GlobalValue::ExternalLinkage,
                                   (*v->fn_name->getIdentifier())->name, &mod);
        functions[v->getUIdentifier()] = parent;
      }
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
  }

  void visitFunctionDeclaration(FunctionDeclaration *v) override {
    if (!v->isFuncPtr) {
      functions[v->getUIdentifier()] =
          llvm::Function::Create(v->getUType()->getLLVMFunctionType(builder),
                                 llvm::GlobalValue::ExternalLinkage,
                                 (*v->fn_name->getIdentifier())->name, &mod);
    }
  }

  void visitDataDeclaration(DataDeclaration *v) override {
    if (!v->global) {
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *dec =
          allocBuilder.CreateAlloca(v->getUType()->getLLVMType(builder));
      dec->setName((*v->data_name->getIdentifier())->name);
      declarations[v->getUIdentifier()] = dec;
    } else {
      llvm::GlobalVariable *dec = new llvm::GlobalVariable(
          mod, v->getUType()->getLLVMType(builder), false,
          llvm::GlobalValue::CommonLinkage,
          llvm::Constant::getNullValue(v->getUType()->getLLVMType(builder)),
          (*v->data_name->getIdentifier())->name);
      declarations[v->getUIdentifier()] = dec;
    }
  }

  void visitStructDeclaration(StructDeclaration *v) override { (void)v; }

  void visitParamDeclaration(ParamDeclaration *v) override { (void)v; }

  void visitScalarType(ScalarType *v) override { (void)v; }

  void visitAbstractType(AbstractType *v) override { (void)v; }

  void visitStructType(StructType *v) override { (void)v; }

  void visitDirectDeclarator(DirectDeclarator *v) override { (void)v; }

  void visitAbstractDeclarator(AbstractDeclarator *v) override { (void)v; }

  void visitPointerDeclarator(PointerDeclarator *v) override { (void)v; }

  void visitFunctionDeclarator(FunctionDeclarator *v) override {
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
        llvm::BasicBlock::Create(ctx, "if.header", parent, nullptr);
    llvm::BasicBlock *IfConsequenceBlock =
        llvm::BasicBlock::Create(ctx, "if.consequence", parent, nullptr);
    llvm::BasicBlock *IfAlternativeBlock =
        llvm::BasicBlock::Create(ctx, "if.alternative", parent, nullptr);
    llvm::BasicBlock *IfEndBlock =
        llvm::BasicBlock::Create(ctx, "if.end", parent, nullptr);
    builder.CreateBr(IfHeaderBlock);
    builder.SetInsertPoint(IfHeaderBlock);
    v->condition->accept(this);
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0), "condition");
    builder.CreateCondBr(c, IfConsequenceBlock, IfAlternativeBlock);
    builder.SetInsertPoint(IfConsequenceBlock);
    v->ifStmt->accept(this);
    builder.CreateBr(IfEndBlock);
    builder.SetInsertPoint(IfAlternativeBlock);
    if (v->elseStmt)
      v->elseStmt->accept(this);
    builder.CreateBr(IfEndBlock);
    builder.SetInsertPoint(IfEndBlock);
  }

  void visitLabel(Label *v) override {
    llvm::BasicBlock *l = llvm::BasicBlock::Create(
        ctx, "label." + v->label_name->name, parent, nullptr);
    labels[v->label_name->name] = l;
    builder.CreateBr(l);
    builder.SetInsertPoint(l);
    v->stmt->accept(this);
  }

  void visitWhile(While *v) override {
    llvm::BasicBlock *whileHeaderBlock =
        llvm::BasicBlock::Create(ctx, "while.header", parent, nullptr);
    llvm::BasicBlock *whileBodyBlock =
        llvm::BasicBlock::Create(ctx, "while.body", parent, nullptr);
    llvm::BasicBlock *whileEndBlock =
        llvm::BasicBlock::Create(ctx, "while.end", parent, nullptr);
    continues.push_back(whileHeaderBlock);
    breaks.push_back(whileEndBlock);
    builder.CreateBr(whileHeaderBlock);
    builder.SetInsertPoint(whileHeaderBlock);
    v->predicate->accept(this);
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0), "condition");
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
    llvm::BasicBlock *b = llvm::BasicBlock::Create(
        ctx, "goto." + v->label_name->name, parent, nullptr);
    builder.CreateBr(b);
    ulabels[v->label_name->name] = b;
  }

  void visitExpressionStmt(ExpressionStmt *v) override {
    v->expr->accept(this);
  }

  void visitBreak(Break *) override { builder.CreateBr(breaks.back()); }

  void visitReturn(Return *v) override {
    if (v->expr) {
      v->expr->accept(this);
      rec_val = builder.CreateZExt(
          rec_val, builder.getCurrentFunctionReturnType(), "convert.ret");
      builder.CreateRet(rec_val);
    } else
      builder.CreateRetVoid();
  }

  void visitContinue(Continue *) override {
    builder.CreateBr(continues.back());
  }

  void visitVariableName(VariableName *v) override {
    if (functions[v->getUIdentifier()])
      rec_val = functions[v->getUIdentifier()];
    else {
      load = declarations[v->getUIdentifier()];
      rec_val = builder.CreateLoad(load, v->name); // move to inner
    }
  }

  void visitNumber(Number *v) override {
    rec_val = builder.getInt32(static_cast<uint32_t>(v->num_value));
  }

  void visitCharacter(Character *v) override {
    rec_val = builder.getInt32(static_cast<uint32_t>(v->char_value));
  }

  void visitString(String *v) override {
    rec_val = builder.CreateGlobalString(v->str_value, "string");
  }

  void visitMemberAccessOp(MemberAccessOp *v) override { (void)v; }

  void visitArraySubscriptOp(ArraySubscriptOp *v) override { (void)v; }

  void visitFunctionCall(FunctionCall *v) override {
    std::vector<llvm::Value *> args;
    for (const auto &a : v->callee_args) {
      a->accept(this);
      args.push_back(rec_val);
    }
    v->callee_name->accept(this);
    if (v->getUType()->getLLVMType(builder) == builder.getVoidTy())
      builder.CreateCall(rec_val, args);
    else
      rec_val = builder.CreateCall(rec_val, args, "call");
  }

  void visitUnary(Unary *v) override {
    v->operand->accept(this);
    rec_val = builder.CreateNeg(rec_val, "unary.minus");
  }

  void visitSizeOf(SizeOf *v) override {
    switch (v->getUType()->getRawTypeValue()) {
    case RawTypeValue::POINTER:
      rec_val = builder.getInt32(8);
      if (v->operand && v->operand->getString())
        rec_val = builder.getInt32(
            static_cast<uint32_t>(v->operand->getString()->str_value.size()));
      break;
    case RawTypeValue::NIL:
      rec_val = builder.getInt32(4);
      break;
    case RawTypeValue::INT:
      rec_val = builder.getInt32(4);
      break;
    case RawTypeValue::CHAR:
      rec_val = builder.getInt32(1);
      break;
    default:
      break;
    }
  }

  void visitBinary(Binary *v) override {
    v->left_operand->accept(this);
    auto lhs = rec_val;
    v->right_operand->accept(this);
    auto rhs = rec_val;
    if (lhs->getType() != rhs->getType()) { // TODO
      lhs = builder.CreateZExt(lhs, builder.getInt32Ty(), "convert.i32");
      rhs = builder.CreateZExt(rhs, builder.getInt32Ty(), "convert.i32");
    }
    switch (v->op_kind) {
    case BinaryOpValue::LESS_THAN:
      rec_val = builder.CreateICmpSLT(lhs, rhs, "binary.less");
      rec_val =
          builder.CreateZExt(rec_val, builder.getInt32Ty(), "convert.bool");
      break;
    case BinaryOpValue::MULTIPLY:
      rec_val = builder.CreateMul(lhs, rhs, "binary.multiply");
      break;
    case BinaryOpValue::ADD:
      rec_val = builder.CreateAdd(lhs, rhs, "binary.add");
      break;
    case BinaryOpValue::SUBTRACT:
      rec_val = builder.CreateSub(lhs, rhs, "binary.sub");
      break;
    case BinaryOpValue::EQUAL:
      rec_val = builder.CreateICmpEQ(lhs, rhs, "binary.euqal");
      rec_val =
          builder.CreateZExt(rec_val, builder.getInt32Ty(), "convert.bool");
      break;
    case BinaryOpValue::NOT_EQUAL:
      rec_val = builder.CreateICmpNE(lhs, rhs, "binary.not");
      rec_val =
          builder.CreateZExt(rec_val, builder.getInt32Ty(), "convert.bool");
      break;
    case BinaryOpValue::LOGICAL_AND:
      rec_val = builder.CreateAnd(lhs, rhs, "binary.and");
      rec_val =
          builder.CreateZExt(rec_val, builder.getInt32Ty(), "convert.bool");
      break;
    case BinaryOpValue::LOGICAL_OR:
      rec_val = builder.CreateOr(lhs, rhs, "binary.or");
      rec_val =
          builder.CreateZExt(rec_val, builder.getInt32Ty(), "convert.bool");
      break;
    case BinaryOpValue::ASSIGN:
      break;
    }
  }

  void visitTernary(Ternary *v) override {
    llvm::BasicBlock *ternaryHeaderBlock =
        llvm::BasicBlock::Create(ctx, "ternary.header", parent, nullptr);
    llvm::BasicBlock *ternaryConsequenceBlock =
        llvm::BasicBlock::Create(ctx, "ternary.consequence", parent, nullptr);
    llvm::BasicBlock *ternaryAlternativeBlock =
        llvm::BasicBlock::Create(ctx, "ternary.alternative", parent, nullptr);
    llvm::BasicBlock *ternaryEndBlock =
        llvm::BasicBlock::Create(ctx, "ternary.end", parent, nullptr);
    builder.CreateBr(ternaryHeaderBlock);
    builder.SetInsertPoint(ternaryHeaderBlock);
    v->predicate->accept(this);
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0), "condition");
    builder.CreateCondBr(c, ternaryConsequenceBlock, ternaryAlternativeBlock);
    builder.SetInsertPoint(ternaryConsequenceBlock);
    v->left_branch->accept(this);
    allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                allocBuilder.GetInsertBlock()->begin());
    llvm::Value *tmp = allocBuilder.CreateAlloca(rec_val->getType());
    tmp->setName("ternary.val");
    builder.CreateStore(rec_val, tmp);
    builder.CreateBr(ternaryEndBlock);
    builder.SetInsertPoint(ternaryAlternativeBlock);
    v->right_branch->accept(this);
    builder.CreateStore(rec_val, tmp);
    builder.CreateBr(ternaryEndBlock);
    builder.SetInsertPoint(ternaryEndBlock);
    rec_val = builder.CreateLoad(tmp);
  }

  void visitAssignment(Assignment *v) override {
    v->left_operand->accept(this);
    auto lhs = load;
    v->right_operand->accept(this);
    auto rhs = rec_val;
    builder.CreateStore(rhs, lhs);
    rec_val = rhs;
  }
};

} // namespace ccc

#endif // C4_CODEGEN_VISITOR_HPP
