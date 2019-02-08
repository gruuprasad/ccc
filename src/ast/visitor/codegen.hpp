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
/**
 * AST visitor class to generate LLVM IR - requires information from  semantical
 * analysis, so only run afterwards
 */
class CodegenVisitor : public Visitor<void> {
  // constants
  llvm::LLVMContext ctx;
  llvm::Module mod;
  llvm::IRBuilder<> builder, allocBuilder;
  // current root of function body
  llvm::Function *parent = nullptr;
  // saved in module and dumped with LLVM IR
  std::string filename;
  // save current break / continue block when entering a loop
  std::vector<llvm::BasicBlock *> breaks;
  std::vector<llvm::BasicBlock *> continues;
  // use temporay blocks for labeling
  std::unordered_map<std::string, llvm::BasicBlock *> labels;
  std::unordered_map<std::string, llvm::BasicBlock *> ulabels;
  // maps for all functions / declarations in file, identified by prefix
  std::unordered_map<std::string, llvm::Value *> declarations;
  std::unordered_map<std::string, llvm::Function *> functions;
  // value pointers for handling of objects, set while traversing the
  // AST recursivly from bottom up
  llvm::Value *rec_val = nullptr;
  llvm::Value *load = nullptr;

public:
  /**
   * pass filename to constructor
   *
   * @param f filename
   */
  explicit CodegenVisitor(std::string f)
      : mod(f, ctx), builder(ctx), allocBuilder(ctx), filename(std::move(f)){};
  ~CodegenVisitor() override = default;

  /**
   * enable external dumping of module to cerr
   */
  void dump() { mod.dump(); }

  /**
   * dump LLVM IR of generated module in file
   */
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

  /**
   * root of AST
   *
   * @param v visitor
   */
  void visitTranslationUnit(TranslationUnit *v) override {
    for (const auto &e : v->extern_list)
      e->accept(this);
  }

  /**
   * define a global function, which can be accessed through the functions map
   *
   * @param v visitor
   */
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

  /**
   * only predeclare a function, as we don't support function pointer
   *
   * @param v visitor
   */
  void visitFunctionDeclaration(FunctionDeclaration *v) override {
    if (!v->isFuncPtr) {
      functions[v->getUIdentifier()] =
          llvm::Function::Create(v->getUType()->getLLVMFunctionType(builder),
                                 llvm::GlobalValue::ExternalLinkage,
                                 (*v->fn_name->getIdentifier())->name, &mod);
    }
  }

  /**
   * create LLVM IR for common declarations
   *
   * @param v visitor
   */
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

  /**
   *
   * @param v visitor
   */
  void visitStructDeclaration(StructDeclaration *v) override {
    (void)v; // TODO WIP
  }

  void visitParamDeclaration(ParamDeclaration *) override {
    // EMPTY
  }

  void visitScalarType(ScalarType *) override {
    // EMPTY
  }

  void visitAbstractType(AbstractType *) override {
    // EMPTY
  }

  /**
   *
   * @param v visitor
   */
  void visitStructType(StructType *v) override {
    (void)v; // TODO WIP
  }

  void visitDirectDeclarator(DirectDeclarator *) override {
    // EMPTY
  }

  void visitAbstractDeclarator(AbstractDeclarator *) override {
    // EMPTY
  }

  void visitPointerDeclarator(PointerDeclarator *) override {
    // EMPTY
  }

  /**
   * used in function definition to generate entry point
   *
   * @param v visitor
   */
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

  /**
   * iterate over children without the need of an own block
   *
   * @param v visitor
   */
  void visitCompoundStmt(CompoundStmt *v) override {
    for (const auto &s : v->block_items)
      s->accept(this);
  }

  /**
   * create branch instructions
   *
   * @param v visitor
   */
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
      rec_val = builder.CreateIsNotNull(rec_val, "notnull");
    rec_val =
        builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
    // always comapare condition to false (accepting all nonzero values as
    // true)
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

  /**
   * insert a block which can be jumped to
   *
   * @param v visitor
   */
  void visitLabel(Label *v) override {
    llvm::BasicBlock *l = llvm::BasicBlock::Create(
        ctx, "label." + v->label_name->name, parent, nullptr);
    labels[v->label_name->name] = l;
    builder.CreateBr(l);
    builder.SetInsertPoint(l);
    v->stmt->accept(this);
  }

  /**
   * gernate LLVM IR of loop instruction
   *
   * @param v visitor
   */
  void visitWhile(While *v) override {
    llvm::BasicBlock *whileHeaderBlock =
        llvm::BasicBlock::Create(ctx, "while.header", parent, nullptr);
    llvm::BasicBlock *whileBodyBlock =
        llvm::BasicBlock::Create(ctx, "while.body", parent, nullptr);
    llvm::BasicBlock *whileEndBlock =
        llvm::BasicBlock::Create(ctx, "while.end", parent, nullptr);
    // save continue / break points
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

  /**
   * insert a empty block that only gets a jump instruction in the end - in that
   * way handling undefined labels
   *
   * @param v visitor
   */
  void visitGoto(Goto *v) override {
    (void)v;
    llvm::BasicBlock *b = llvm::BasicBlock::Create(
        ctx, "goto." + v->label_name->name, parent, nullptr);
    builder.CreateBr(b);
    ulabels[v->label_name->name] = b;
  }

  /**
   * nothing to do
   *
   * @param v visitor
   */
  void visitExpressionStmt(ExpressionStmt *v) override {
    if (v->expr)
      v->expr->accept(this);
  }

  /**
   * jump to loop end
   *
   */
  void visitBreak(Break *) override { builder.CreateBr(breaks.back()); }

  /**
   * generate return value, insert a dead block afterwards
   *
   * @param v visitor
   */
  void visitReturn(Return *v) override {
    if (v->expr) {
      v->expr->accept(this);
      rec_val = builder.CreateZExtOrTrunc(
          rec_val, builder.getCurrentFunctionReturnType(), "ret");
      builder.CreateRet(rec_val);
    } else
      builder.CreateRet(llvm::Constant::getNullValue(parent->getReturnType()));
    llvm::BasicBlock *ReturnDeadBlock =
        llvm::BasicBlock::Create(ctx, "DEAD_BLOCK", parent, nullptr);
    builder.SetInsertPoint(ReturnDeadBlock);
  }

  /**
   * jump to loop header
   *
   */
  void visitContinue(Continue *) override {
    builder.CreateBr(continues.back());
  }

  /**
   * perform lookup of object in global maps
   *
   * @param v visitor
   */
  void visitVariableName(VariableName *v) override {
    if (functions[v->getUIdentifier()])
      rec_val = functions[v->getUIdentifier()];
    else {
      load = declarations[v->getUIdentifier()];
      rec_val = builder.CreateLoad(load, v->name);
    }
  }

  /**
   * get i32 of value
   *
   * @param v visitor
   */
  void visitNumber(Number *v) override {
    rec_val = builder.getInt32(static_cast<uint32_t>(v->num_value));
  }

  /**
   * @param v visitor
   */
  void visitCharacter(Character *v) override {
    unsigned int val = 0;
    // value of escaped character
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

  /**
   * @param v visitor
   */
  void visitString(String *v) override {
    std::stringstream ss;
    for (unsigned int i = 0; i < v->str_value.size(); i++) {
      // replace escaped character
      if (v->str_value[i] == '\\') {
        switch (v->str_value[++i]) {
        case 'a':
          ss << '\a';
          break;
        case 'b':
          ss << '\b';
          break;
        case 'f':
          ss << '\f';
          break;
        case 'n':
          ss << '\n';
          break;
        case 'r':
          ss << '\r';
          break;
        case 't':
          ss << '\t';
          break;
        case 'v':
          ss << '\v';
          break;
        case '\\':
          ss << '\\';
          break;
        case '\'':
          ss << '\'';
          break;
        case '\"':
          ss << '\"';
          break;
        case '?':
          ss << '\?';
          break;
        case '0':
          ss << '\0';
          break;
        default:
          break;
        }
      } else
        ss << v->str_value[i];
    }
    rec_val = builder.CreateGlobalString(ss.str(), "string");
    rec_val = builder.CreatePointerBitCastOrAddrSpaceCast(
        rec_val, builder.getInt8PtrTy(), "cast");
  }

  /**
   * @param v visitor
   */
  void visitMemberAccessOp(MemberAccessOp *v) override {
    (void)v; // TODO WIP
  }

  /**
   * calcualte element pointer with offset value
   *
   * @param v visitor
   */
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

  /**
   * lookup function in map and pass arguments
   *
   * @param v visitor
   */
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

  /**
   * gernate code for unary expression
   *
   * @param v visitor
   */
  void visitUnary(Unary *v) override {
    v->operand->accept(this);
    switch (v->op_kind) {
    case UnaryOpValue::ADDRESS_OF:
      // already loaded
      rec_val = load;
      break;
    case UnaryOpValue::DEREFERENCE:
      // points to object
      load = rec_val;
      rec_val = builder.CreateLoad(rec_val, "deref");
      break;
    case UnaryOpValue::MINUS:
      // only appears for numbers
      rec_val = builder.CreateNeg(rec_val, "minus");
      break;
    case UnaryOpValue::NOT:
      // pointer not null
      if (v->operand->getUType()->getRawTypeValue() == RawTypeValue::POINTER) {
        rec_val = builder.CreateIsNull(rec_val, "isnull");
      } else {
        // negate boolean value
        rec_val =
            builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
        rec_val = builder.CreateICmpEQ(rec_val, builder.getInt32(0), "cmpne");
      }
      // return boolean value
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    }
  }

  /**
   * @param v visitor
   */
  void visitSizeOf(SizeOf *v) override {
    if (v->operand)
      // expression
      rec_val = builder.getInt32(
          static_cast<uint32_t>(v->operand->getUType()->size()));
    else if (v->type_name->getUType())
      // type
      rec_val = builder.getInt32(
          static_cast<uint32_t>(v->type_name->getUType()->size()));
    else
      rec_val = builder.getInt32(0);
    // sizeof sizeof
    if (v->operand && v->operand->isSizeOf()) {
      rec_val = builder.getInt32(8);
    } else if (v->operand && v->operand->getString()) {
      // calculate length of string without escape sequences but with tailing
      // \0
      auto str = v->operand->getString()->str_value;
      str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());
      rec_val = builder.getInt32(static_cast<uint32_t>(str.size() + 1));
    }
  }

  /**
   * gernate code for binary expressions
   *
   * @param v visitor
   */
  void visitBinary(Binary *v) override {
    llvm::Value *lhs = nullptr;
    llvm::Value *rhs = nullptr;
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
      // always return i32
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
      // pointer arithmetic
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER) {
        load = builder.CreateGEP(lhs, rhs, "gep");
        rec_val = load;
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::POINTER) {
        load = builder.CreateGEP(rhs, lhs, "gep");
        rec_val = load;
      } else {
        // add numbers
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
      // pointer arithmetic
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
          load = builder.CreateGEP(rhs, lhs, "gep");
          rec_val = load;
        } else if (v->right_operand->getUType()->getRawTypeValue() ==
                   RawTypeValue::INT) {
          rhs = builder.CreateNeg(rhs, "minus");
          load = builder.CreateGEP(lhs, rhs, "gep");
          rec_val = load;
        } else {
          // ptrdiff
          lhs = builder.CreatePtrToInt(lhs, builder.getInt32Ty(), "i32");
          rhs = builder.CreatePtrToInt(rhs, builder.getInt32Ty(), "i32");
          rec_val = builder.CreateSub(lhs, rhs, "sub");
          rec_val = builder.CreateExactSDiv(
              rec_val,
              builder.getInt32(static_cast<uint32_t>(
                  v->left_operand->getUType()->ptr_size())),
              "div");
          rec_val = builder.CreateTrunc(rec_val, builder.getInt32Ty(), "trunc");
        }
      } else {
        // subtract numbers
        if (v->left_operand->getUType()->getRawTypeValue() ==
                RawTypeValue::CHAR &&
            v->right_operand->getUType()->getRawTypeValue() ==
                RawTypeValue::CHAR) {
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
      // compare pointer
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
        rec_val = builder.CreateIsNull(rhs, "isnull");
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNull(lhs, "isnull");
      } else {
        // compare values
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateICmpEQ(lhs, rhs, "euqal");
      }
      // return i32
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    case BinaryOpValue::NOT_EQUAL:
      v->left_operand->accept(this);
      lhs = rec_val;
      v->right_operand->accept(this);
      rhs = rec_val;
      // compare pointer
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
        rec_val = builder.CreateIsNotNull(rhs, "notnull");
      } else if (v->right_operand->getUType()->getRawTypeValue() ==
                 RawTypeValue::NIL) {
        rec_val = builder.CreateIsNotNull(lhs, "notnull");
      } else {
        // compare values
        if (lhs->getType()->isIntegerTy(8) && rhs->getType()->isIntegerTy(8)) {
        } else {
          lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
          rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        }
        rec_val = builder.CreateICmpNE(lhs, rhs, "cmpne");
      }
      // return i32
      rec_val =
          builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
      break;
    case BinaryOpValue::LOGICAL_AND: {
      llvm::BasicBlock *lazy_h =
          llvm::BasicBlock::Create(ctx, "land.rhs", parent, nullptr);
      llvm::BasicBlock *lazy_e =
          llvm::BasicBlock::Create(ctx, "land.end", parent, nullptr);
      // store result of lazy evaluation
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *tmp = allocBuilder.CreateAlloca(builder.getInt1Ty());
      tmp->setName("lazy");
      v->left_operand->accept(this);
      lhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        lhs = builder.CreateIsNotNull(lhs, "notnull");
      else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        lhs = builder.CreateICmpNE(lhs, builder.getInt32(0), "cmp");
      }
      builder.CreateStore(lhs, tmp);
      // evaluate right expression only if left returned true
      builder.CreateCondBr(lhs, lazy_h, lazy_e);
      builder.SetInsertPoint(lazy_h);
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->right_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        rhs = builder.CreateIsNotNull(rhs, "notnull");
      else {
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateICmpNE(rhs, builder.getInt32(0), "cmp");
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
      // store result of lazy evaluation
      allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                  allocBuilder.GetInsertBlock()->begin());
      llvm::Value *tmp = allocBuilder.CreateAlloca(builder.getInt1Ty());
      tmp->setName("lazy");
      v->left_operand->accept(this);
      lhs = rec_val;
      if (v->left_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        lhs = builder.CreateIsNotNull(lhs, "notnull");
      else {
        lhs = builder.CreateZExtOrBitCast(lhs, builder.getInt32Ty(), "zext");
        lhs = builder.CreateICmpNE(lhs, builder.getInt32(0), "cmp");
      }
      builder.CreateStore(lhs, tmp);
      // evaluate right expression only if left returned false
      builder.CreateCondBr(lhs, lazy_e, lazy_h);
      builder.SetInsertPoint(lazy_h);
      v->right_operand->accept(this);
      rhs = rec_val;
      if (v->right_operand->getUType()->getRawTypeValue() ==
          RawTypeValue::POINTER)
        rhs = builder.CreateIsNotNull(rhs, "notnull");
      else {
        rhs = builder.CreateZExtOrBitCast(rhs, builder.getInt32Ty(), "zext");
        rhs = builder.CreateICmpNE(rhs, builder.getInt32(0), "cmp");
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

  /**
   * gernate code for ternary expression
   *
   * @param v visitor
   */
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
    // gernate conditional branching
    v->predicate->accept(this);
    if (v->predicate->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
      rec_val = builder.CreateIsNotNull(rec_val);
    rec_val =
        builder.CreateZExtOrBitCast(rec_val, builder.getInt32Ty(), "zext");
    auto c = builder.CreateICmpNE(rec_val, builder.getInt32(0), "condition");
    builder.CreateCondBr(c, ternaryConsequenceBlock, ternaryAlternativeBlock);
    builder.SetInsertPoint(ternaryConsequenceBlock);
    // calculate result type of both branches
    llvm::Type *type = builder.getInt32Ty();
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::CHAR &&
        v->right_branch->getUType()->getRawTypeValue() == RawTypeValue::CHAR)
      type = builder.getInt8Ty();
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::POINTER)
      type = v->left_branch->getUType()->getLLVMType(builder);
    else if (v->right_branch->getUType()->getRawTypeValue() ==
             RawTypeValue::POINTER)
      type = v->right_branch->getUType()->getLLVMType(builder);
    // use a temporay variable to store result of branch evaluation
    allocBuilder.SetInsertPoint(allocBuilder.GetInsertBlock(),
                                allocBuilder.GetInsertBlock()->begin());
    llvm::Value *tmp = allocBuilder.CreateAlloca(type);
    tmp->setName("ternary.val");
    // calculate result of left branch
    if (v->left_branch->getUType()->getRawTypeValue() == RawTypeValue::NIL)
      rec_val = llvm::Constant::getNullValue(type);
    else {
      v->left_branch->accept(this);
      rec_val = builder.CreateZExtOrTrunc(rec_val, type);
    }
    builder.CreateStore(rec_val, tmp);
    builder.CreateBr(ternaryEndBlock);
    // calculate result of right branch
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
    // load result which contains value of executed branch
    rec_val = builder.CreateLoad(tmp);
  }

  /**
   *  assign rhs to lhs by handling pointer to object (requires load to be set
   * to storage point) and cast accordingly (void* etc.)
   *
   * @param v visitor
   */
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
    else if (v->left_operand->getUType()->getRawTypeValue() ==
             RawTypeValue::POINTER)
      rhs = builder.CreatePointerBitCastOrAddrSpaceCast(
          rhs, v->left_operand->getUType()->getLLVMType(builder), "cast");
    builder.CreateStore(rhs, lhs);
    rec_val = rhs;
  }
};
} // namespace ccc
#endif // C4_CODEGEN_VISITOR_HPP
