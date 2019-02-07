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
  llvm::Function *parent = nullptr;

  std::string filename;

  std::vector<llvm::BasicBlock *> breaks;
  std::vector<llvm::BasicBlock *> continues;
  std::unordered_map<std::string, llvm::BasicBlock *> labels;
  std::unordered_map<std::string, llvm::BasicBlock *> ulabels;
  std::unordered_map<std::string, llvm::Value *> declarations;
  std::unordered_map<std::string, llvm::Function *> functions;

  llvm::Value *rec_val = nullptr;
  llvm::Value *load = nullptr;

public:
  explicit CodegenVisitor(std::string f)
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
        builder.CreateRet(llvm::Constant::getNullValue(CurFuncReturnType));
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
    } else if (declarations.find(v->getUIdentifier()) == declarations.end()) {
      llvm::GlobalVariable *dec = new llvm::GlobalVariable(
          mod, v->getUType()->getLLVMType(builder), false,
          llvm::GlobalValue::CommonLinkage,
          llvm::Constant::getNullValue(v->getUType()->getLLVMType(builder)),
          (*v->data_name->getIdentifier())->name);
      declarations[v->getUIdentifier()] = dec;
    }
  }

  void visitStructDeclaration(StructDeclaration *v) override { (void)v; }

  void visitParamDeclaration(ParamDeclaration *) override {
    // EMPTY
  }

  void visitScalarType(ScalarType *) override {
    // EMPTY
  }

  void visitAbstractType(AbstractType *) override {
    // EMPTY
  }

  void visitStructType(StructType *v) override { (void)v; }

  void visitDirectDeclarator(DirectDeclarator *) override {
    // EMPTY
  }

  void visitAbstractDeclarator(AbstractDeclarator *) override {
    // EMPTY
  }

  void visitPointerDeclarator(PointerDeclarator *) override {
    // EMPTY
  }

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
      ArgVarAPtr->setName(a.getName());
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
    if (v->condition->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
      rec_val = builder.CreateIsNotNull(rec_val, "nn");
    rec_val =
        builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
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
    rec_val =
        builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
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
    if (v->expr)
      v->expr->accept(this);
  }

  void visitBreak(Break *) override { builder.CreateBr(breaks.back()); }

  void visitReturn(Return *v) override {
    if (v->expr) {
      v->expr->accept(this);
      rec_val = builder.CreateZExtOrTrunc(
          rec_val, builder.getCurrentFunctionReturnType(), "ret");
      builder.CreateRet(rec_val);
    } else
      builder.CreateRet(llvm::Constant::getNullValue(parent->getReturnType()));
  }

  void visitContinue(Continue *) override {
    builder.CreateBr(continues.back());
  }

  void visitVariableName(VariableName *v) override {
    if (functions[v->getUIdentifier()])
      rec_val = functions[v->getUIdentifier()];
    else {
      load = declarations[v->getUIdentifier()];
      rec_val = builder.CreateLoad(load, v->name);
    }
  }

  void visitNumber(Number *v) override {
    rec_val = builder.getInt32(static_cast<uint32_t>(v->num_value));
  }

  void visitCharacter(Character *v) override {
    unsigned int val = 0;
    if (v->char_value[0] == '\\') {
      switch (v->char_value[1]) {
      case 'a':
        val = 7;
        break;
      case 'b':
        val = 8;
        break;
      case 'f':
        val = 12;
        break;
      case 'n':
        val = 10;
        break;
      case 'r':
        val = 13;
        break;
      case 't':
        val = 9;
        break;
      case 'v':
        val = 11;
        break;
      case '\\':
        val = 92;
        break;
      case '\'':
        val = 39;
        break;
      case '\"':
        val = 34;
        break;
      case '?':
        val = 63;
        break;
      case '0':
        val = 0;
        break;
      default:
        break;
      }
    } else
      val = (unsigned int)(v->char_value[0]);
    rec_val = builder.getInt32(val);
  }
  void visitString(String *v) override {
    rec_val = builder.CreateGlobalString(v->str_value, "string");
    //    llvm::GlobalVariable *str = new llvm::GlobalVariable(
    //        mod, rec_val->getType(), false,
    //        llvm::GlobalValue::CommonLinkage,
    //        llvm::Constant::getNullValue(rec_val->getType()), "string");
    //    rec_val = builder.CreateStore(rec_val, str, "store.string");
    rec_val = builder.CreatePointerBitCastOrAddrSpaceCast(
        rec_val, builder.getInt8PtrTy(), "cast");
  }

  void visitMemberAccessOp(MemberAccessOp *v) override { (void)v; } // TODO

  void visitArraySubscriptOp(ArraySubscriptOp *v) override {
    v->array_name->accept(this);
    auto callee = rec_val;
    v->index_value->accept(this);
    auto index = rec_val;
    if (callee->getType()->isIntegerTy())
      load = builder.CreateGEP(index, callee, "idx");
    else
      load = builder.CreateGEP(callee, index, "idx");
    rec_val = builder.CreateLoad(load, "array");
  }

  void visitFunctionCall(FunctionCall *v) override {
    v->callee_name->accept(this);
    auto callee = rec_val;
    std::vector<llvm::Value *> args;
    unsigned int pos = 0;
    for (const auto &a : v->callee_args) {
      a->accept(this);
      if (a->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
        rec_val = builder.CreateBitOrPointerCast(
            rec_val, a->getUType()->getLLVMType(builder), "cast");
      else
        rec_val = builder.CreateZExtOrTrunc(
            rec_val, a->getUType()->getLLVMType(builder), "zext");
      pos++;
      args.push_back(rec_val);
    }
    rec_val = builder.CreateCall(callee, args, "call");
  }

  void visitUnary(Unary *v) override {
    v->operand->accept(this);
    switch (v->op_kind) {
    case UnaryOpValue::ADDRESS_OF:
      rec_val = load;
      break;
    case UnaryOpValue::DEREFERENCE:
      load = rec_val;
      rec_val = builder.CreateLoad(rec_val, "deref");
      break;
    case UnaryOpValue::MINUS:
      rec_val = builder.CreateNeg(rec_val, "minus");
      break;
    case UnaryOpValue::NOT:
      if (v->operand->getUType()->getRawTypeValue() == RawTypeValue::POINTER) {
        rec_val = builder.CreateIsNull(rec_val, "null");
      } else {
        rec_val =
            builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
        rec_val = builder.CreateICmpEQ(rec_val, builder.getInt32(0), "not");
      }
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    }
  }

  void visitSizeOf(SizeOf *v) override {
    if (v->operand)
      rec_val = builder.getInt32(
          static_cast<uint32_t>(v->operand->getUType()->size()));
    else
      rec_val = builder.getInt32(
          static_cast<uint32_t>(v->type_name->getUType()->size()));
    if (v->operand && v->operand->isSizeOf()) {
      rec_val = builder.getInt32(8);
    } else if (v->operand && v->operand->getString()) {
      auto str = v->operand->getString()->str_value;
      str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());
      rec_val = builder.getInt32(static_cast<uint32_t>(str.size() + 1));
    }
  }

  void visitBinary(Binary *v) override {
    llvm::Value *lhs = nullptr;
    llvm::Value *rhs = rec_val;
    switch (v->op_kind) {
    case BinaryOpValue::LESS_THAN:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
      } else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
      }
      rec_val = builder.CreateICmpSLT(lhs, rhs, "less");
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    case BinaryOpValue::MULTIPLY:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
      } else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
      }
      rec_val = builder.CreateMul(lhs, rhs, "multiply");
      break;
    case BinaryOpValue::ADD:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER) {
        load = builder.CreateGEP(lhs, rhs, "ptr");
        rec_val = load;
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::POINTER) {
        load = builder.CreateGEP(rhs, lhs, "ptr");
        rec_val = load;
      } else {
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateAdd(lhs, rhs, "add");
      }
      break;
    case BinaryOpValue::SUBTRACT:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER ||
          v->right_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER) {
        if (v->left_operand->getUType()->getRawTypeValue() ==
            RawTypeValue::NIL) {
          rec_val = rhs;
          break;
        } else if (v->right_operand->getUType()->getRawTypeValue() ==
                   RawTypeValue::NIL) {
          rec_val = lhs;
          break;
        } else if (v->left_operand->getUType()->getRawTypeValue() ==
                   RawTypeValue::INT) {
          lhs = builder.CreateNeg(lhs, "minus");
          load = builder.CreateGEP(rhs, lhs, "ptr");
          rec_val = load;
        } else if (v->right_operand->getUType()->getRawTypeValue() ==
                   RawTypeValue::INT) {
          rhs = builder.CreateNeg(rhs, "minus");
          load = builder.CreateGEP(lhs, rhs, "ptr");
          rec_val = load;
        } else {
          lhs = builder.CreatePtrToInt(lhs, builder.getInt32Ty(), "i64");
          rhs = builder.CreatePtrToInt(rhs, builder.getInt32Ty(), "i64");
          rec_val = builder.CreateSub(lhs, rhs, "sub");
          rec_val = builder.CreateExactSDiv(
              rec_val,
              builder.getInt32(static_cast<uint32_t>(
                  v->left_operand->getUType()->ptr_size())),
              "div");
          rec_val = builder.CreateTrunc(rec_val, builder.getInt32Ty(), "trunc");
        }
      } else {
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateSub(lhs, rhs, "sub");
      }
      break;
    case BinaryOpValue::EQUAL:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER &&
          v->right_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER) {
        rec_val = builder.getInt1(lhs->getType() == rhs->getType());
      } else if (v->left_operand->getUType()->getRawTypeValue() ==
                     RawTypeValue::NIL &&
                 v->right_operand->getUType()->getRawTypeValue() ==
                     RawTypeValue::NIL) {
        rec_val = builder.getInt1(true);
      } else if (v->left_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNull(rhs, "null");
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNull(lhs, "null");
      } else {
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateICmpEQ(lhs, rhs, "euqal");
      }
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    case BinaryOpValue::NOT_EQUAL:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER &&
          v->right_operand->getUType()->getRawTypeValue() ==
              RawTypeValue::POINTER) {
        rec_val = builder.getInt1(lhs->getType() != rhs->getType());
      } else if (v->left_operand->getUType()->getRawTypeValue() ==
                     RawTypeValue::NIL &&
                 v->right_operand->getUType()->getRawTypeValue() ==
                     RawTypeValue::NIL) {
        rec_val = builder.getInt1(false);
      } else if (v->left_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNotNull(rhs, "nn");
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNotNull(lhs, "nn");
      } else {
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateICmpNE(lhs, rhs, "not");
      }
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    case BinaryOpValue::LOGICAL_AND: {
      llvm::BasicBlock *lazy_h =
          llvm::BasicBlock::Create(ctx, "land.rhs", parent, nullptr);
      llvm::BasicBlock *lazy_e =
          llvm::BasicBlock::Create(ctx, "land.end", parent, nullptr);
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *tmp = allocBuilder.CreateAlloca(builder.getInt1Ty());
      tmp->setName("lazy");
      v->left_operand->accept(this);
      lhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        lhs = builder.CreateIsNotNull(lhs, "nn");
      else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        lhs = builder.CreateICmpNE(lhs, builder.getInt32(0), "true");
      }
      builder.CreateStore(lhs, tmp);
      builder.CreateCondBr(lhs, lazy_h, lazy_e);
      builder.SetInsertPoint(lazy_h);
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->right_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        rhs = builder.CreateIsNotNull(rhs, "nn");
      else {
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateICmpNE(rhs, builder.getInt32(0), "true");
      }
      builder.CreateStore(rhs, tmp);
      builder.CreateBr(lazy_e);
      builder.SetInsertPoint(lazy_e);
      rec_val = builder.CreateLoad(tmp, "lazy");
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    }
    case BinaryOpValue::LOGICAL_OR: {
      llvm::BasicBlock *lazy_h =
          llvm::BasicBlock::Create(ctx, "lor.rhs", parent, nullptr);
      llvm::BasicBlock *lazy_e =
          llvm::BasicBlock::Create(ctx, "lor.end", parent, nullptr);
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *tmp = allocBuilder.CreateAlloca(builder.getInt1Ty());
      tmp->setName("lazy");
      v->left_operand->accept(this);
      lhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        lhs = builder.CreateIsNotNull(lhs, "nn");
      else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        lhs = builder.CreateICmpNE(lhs, builder.getInt32(0), "true");
      }
      builder.CreateStore(lhs, tmp);
      builder.CreateCondBr(lhs, lazy_e, lazy_h);
      builder.SetInsertPoint(lazy_h);
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->right_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        rhs = builder.CreateIsNotNull(rhs, "nn");
      else {
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateICmpNE(rhs, builder.getInt32(0), "true");
      }
      builder.CreateStore(rhs, tmp);
      builder.CreateBr(lazy_e);
      builder.SetInsertPoint(lazy_e);
      rec_val = builder.CreateLoad(tmp, "lazy");
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    }
    case BinaryOpValue::ASSIGN:
      // EMPTY
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

    if (v->predicate->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
      rec_val = builder.CreateIsNotNull(rec_val);
    rec_val =
        builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0), "condition");
    builder.CreateCondBr(c, ternaryConsequenceBlock, ternaryAlternativeBlock);
    builder.SetInsertPoint(ternaryConsequenceBlock);

    llvm::Type *type = builder.getInt32Ty();
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::CHAR &&
        v->right_branch->getUType()->getRawTypeValue() == RawTypeValue::CHAR)
      type = builder.getInt8Ty();
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
      type = v->left_branch->getUType()->getLLVMType(builder);
    else if (v->right_branch->getUType()->getRawTypeValue() ==
             RawTypeValue::POINTER)
      type = v->right_branch->getUType()->getLLVMType(builder);
    allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                allocBuilder.GetInsertBlock()->begin());
    llvm::Value *tmp = allocBuilder.CreateAlloca(type);
    tmp->setName("ternary.val");
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::NIL)
      rec_val = llvm::Constant::getNullValue(type);
    else {
      v->left_branch->accept(this);
      rec_val = builder.CreateZExtOrTrunc(rec_val, type);
    }
    builder.CreateStore(rec_val, tmp);
    builder.CreateBr(ternaryEndBlock);
    builder.SetInsertPoint(ternaryAlternativeBlock);
    if (v->right_branch->getUType()->getRawTypeValue() == RawTypeValue::NIL)
      rec_val = llvm::Constant::getNullValue(type);
    else {
      v->right_branch->accept(this);
      rec_val = builder.CreateZExtOrTrunc(rec_val, type);
    }
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
    if (v->left_operand->getUType()->getRawTypeValue() == RawTypeValue::INT &&
        v->right_operand->getUType()->getRawTypeValue() == RawTypeValue::CHAR)
      rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
    else if (v->left_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::CHAR &&
             v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::INT)
      rhs = builder.CreateTrunc(rhs, builder.getInt8Ty(), "trunc");
    else if (v->left_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::POINTER &&
             v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL)
      rhs = llvm::Constant::getNullValue(
          v->left_operand->getUType()->getLLVMType(builder));
    if (rhs->getType()->isPointerTy())
      rhs = builder.CreatePointerBitCastOrAddrSpaceCast(
          rhs, v->left_operand->getUType()->getLLVMType(builder), "cast");
    builder.CreateStore(rhs, lhs);
    rec_val = rhs;
  }
};

} // namespace ccc

#endif // C4_CODEGEN_VISITOR_HPP
