#ifndef C4_CODEGEN_VISITOR_HPP
#define C4_CODEGEN_VISITOR_HPP

#include "../ast_node.hpp"

namespace ccc {

class CodegenVisitor : public Visitor {

  std::string error;

public:
  CodegenVisitor() = default;
  ~CodegenVisitor() override = default;

  std::string visitTranslationUnit(TranslationUnit *v) override {
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    return error;
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    return error;
  }

  std::string visitScalarType(ScalarType *v) override { return error; }

  std::string visitAbstractType(AbstractType *v) override { return error; }

  std::string visitStructType(StructType *v) override { return error; }

  std::string visitDirectDeclarator(DirectDeclarator *v) override {
    return error;
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    return error;
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    return error;
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    return error;
  }

  std::string visitCompoundStmt(CompoundStmt *v) override { return error; }

  std::string visitIfElse(IfElse *v) override { return error; }

  std::string visitLabel(Label *v) override { return error; }

  std::string visitWhile(While *v) override { return error; }

  std::string visitGoto(Goto *v) override { return error; }

  std::string visitExpressionStmt(ExpressionStmt *v) override { return error; }

  std::string visitBreak(Break *v) override { return error; }

  std::string visitReturn(Return *v) override { return error; }

  std::string visitContinue(Continue *v) override { return error; }

  std::string visitVariableName(VariableName *v) override { return error; }

  std::string visitNumber(Number *v) override { return error; }

  std::string visitCharacter(Character *v) override { return error; }

  std::string visitString(String *v) override { return error; }

  std::string visitMemberAccessOp(MemberAccessOp *v) override { return error; }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    return error;
  }

  std::string visitFunctionCall(FunctionCall *v) override { return error; }

  std::string visitUnary(Unary *v) override { return error; }

  std::string visitSizeOf(SizeOf *v) override { return error; }

  std::string visitBinary(Binary *v) override { return error; }

  std::string visitTernary(Ternary *v) override { return error; }

  std::string visitAssignment(Assignment *v) override { return error; }
};

} // namespace ccc

#endif // C4_CODEGEN_VISITOR_HPP
