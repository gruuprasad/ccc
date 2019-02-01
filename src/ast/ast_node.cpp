#include "ast_node.hpp"

namespace ccc {

std::string TranslationUnit::accept(Visitor<std::string> *v) {
  return v->visitTranslationUnit(this);
}
std::string FunctionDefinition::accept(Visitor<std::string> *v) {
  return v->visitFunctionDefinition(this);
}
std::string FunctionDeclaration::accept(Visitor<std::string> *v) {
  return v->visitFunctionDeclaration(this);
}
std::string DataDeclaration::accept(Visitor<std::string> *v) {
  return v->visitDataDeclaration(this);
}
std::string StructDeclaration::accept(Visitor<std::string> *v) {
  return v->visitStructDeclaration(this);
}
std::string ParamDeclaration::accept(Visitor<std::string> *v) {
  return v->visitParamDeclaration(this);
}
std::string ScalarType::accept(Visitor<std::string> *v) {
  return v->visitScalarType(this);
}
std::string StructType::accept(Visitor<std::string> *v) {
  return v->visitStructType(this);
}
std::string AbstractType::accept(Visitor<std::string> *v) {
  return v->visitAbstractType(this);
}
std::string DirectDeclarator::accept(Visitor<std::string> *v) {
  return v->visitDirectDeclarator(this);
}
std::string AbstractDeclarator::accept(Visitor<std::string> *v) {
  return v->visitAbstractDeclarator(this);
}
std::string PointerDeclarator::accept(Visitor<std::string> *v) {
  return v->visitPointerDeclarator(this);
}
std::string FunctionDeclarator::accept(Visitor<std::string> *v) {
  return v->visitFunctionDeclarator(this);
}
std::string CompoundStmt::accept(Visitor<std::string> *v) {
  return v->visitCompoundStmt(this);
}
std::string IfElse::accept(Visitor<std::string> *v) {
  return v->visitIfElse(this);
}
std::string Label::accept(Visitor<std::string> *v) {
  return v->visitLabel(this);
}
std::string While::accept(Visitor<std::string> *v) {
  return v->visitWhile(this);
}
std::string Goto::accept(Visitor<std::string> *v) { return v->visitGoto(this); }
std::string ExpressionStmt::accept(Visitor<std::string> *v) {
  return v->visitExpressionStmt(this);
}
std::string Break::accept(Visitor<std::string> *v) {
  return v->visitBreak(this);
}
std::string Return::accept(Visitor<std::string> *v) {
  return v->visitReturn(this);
}
std::string Continue::accept(Visitor<std::string> *v) {
  return v->visitContinue(this);
}
std::string VariableName::accept(Visitor<std::string> *v) {
  return v->visitVariableName(this);
}
std::string Number::accept(Visitor<std::string> *v) {
  return v->visitNumber(this);
}
std::string Character::accept(Visitor<std::string> *v) {
  return v->visitCharacter(this);
}
std::string String::accept(Visitor<std::string> *v) {
  return v->visitString(this);
}
std::string MemberAccessOp::accept(Visitor<std::string> *v) {
  return v->visitMemberAccessOp(this);
}
std::string ArraySubscriptOp::accept(Visitor<std::string> *v) {
  return v->visitArraySubscriptOp(this);
}
std::string FunctionCall::accept(Visitor<std::string> *v) {
  return v->visitFunctionCall(this);
}
std::string Unary::accept(Visitor<std::string> *v) {
  return v->visitUnary(this);
}
std::string SizeOf::accept(Visitor<std::string> *v) {
  return v->visitSizeOf(this);
}
std::string Binary::accept(Visitor<std::string> *v) {
  return v->visitBinary(this);
}
std::string Ternary::accept(Visitor<std::string> *v) {
  return v->visitTernary(this);
}
std::string Assignment::accept(Visitor<std::string> *v) {
  return v->visitAssignment(this);
}
llvm::Value *TranslationUnit::accept(Visitor<llvm::Value *> *v) {
  return v->visitTranslationUnit(this);
}
llvm::Value *FunctionDefinition::accept(Visitor<llvm::Value *> *v) {
  return v->visitFunctionDefinition(this);
}
llvm::Value *FunctionDeclaration::accept(Visitor<llvm::Value *> *v) {
  return v->visitFunctionDeclaration(this);
}
llvm::Value *DataDeclaration::accept(Visitor<llvm::Value *> *v) {
  return v->visitDataDeclaration(this);
}
llvm::Value *StructDeclaration::accept(Visitor<llvm::Value *> *v) {
  return v->visitStructDeclaration(this);
}
llvm::Value *ParamDeclaration::accept(Visitor<llvm::Value *> *v) {
  return v->visitParamDeclaration(this);
}
llvm::Value *ScalarType::accept(Visitor<llvm::Value *> *v) {
  return v->visitScalarType(this);
}
llvm::Value *StructType::accept(Visitor<llvm::Value *> *v) {
  return v->visitStructType(this);
}
llvm::Value *AbstractType::accept(Visitor<llvm::Value *> *v) {
  return v->visitAbstractType(this);
}
llvm::Value *DirectDeclarator::accept(Visitor<llvm::Value *> *v) {
  return v->visitDirectDeclarator(this);
}
llvm::Value *AbstractDeclarator::accept(Visitor<llvm::Value *> *v) {
  return v->visitAbstractDeclarator(this);
}
llvm::Value *PointerDeclarator::accept(Visitor<llvm::Value *> *v) {
  return v->visitPointerDeclarator(this);
}
llvm::Value *FunctionDeclarator::accept(Visitor<llvm::Value *> *v) {
  return v->visitFunctionDeclarator(this);
}
llvm::Value *CompoundStmt::accept(Visitor<llvm::Value *> *v) {
  return v->visitCompoundStmt(this);
}
llvm::Value *IfElse::accept(Visitor<llvm::Value *> *v) {
  return v->visitIfElse(this);
}
llvm::Value *Label::accept(Visitor<llvm::Value *> *v) {
  return v->visitLabel(this);
}
llvm::Value *While::accept(Visitor<llvm::Value *> *v) {
  return v->visitWhile(this);
}
llvm::Value *Goto::accept(Visitor<llvm::Value *> *v) {
  return v->visitGoto(this);
}
llvm::Value *ExpressionStmt::accept(Visitor<llvm::Value *> *v) {
  return v->visitExpressionStmt(this);
}
llvm::Value *Break::accept(Visitor<llvm::Value *> *v) {
  return v->visitBreak(this);
}
llvm::Value *Return::accept(Visitor<llvm::Value *> *v) {
  return v->visitReturn(this);
}
llvm::Value *Continue::accept(Visitor<llvm::Value *> *v) {
  return v->visitContinue(this);
}
llvm::Value *VariableName::accept(Visitor<llvm::Value *> *v) {
  return v->visitVariableName(this);
}
llvm::Value *Number::accept(Visitor<llvm::Value *> *v) {
  return v->visitNumber(this);
}
llvm::Value *Character::accept(Visitor<llvm::Value *> *v) {
  return v->visitCharacter(this);
}
llvm::Value *String::accept(Visitor<llvm::Value *> *v) {
  return v->visitString(this);
}
llvm::Value *MemberAccessOp::accept(Visitor<llvm::Value *> *v) {
  return v->visitMemberAccessOp(this);
}
llvm::Value *ArraySubscriptOp::accept(Visitor<llvm::Value *> *v) {
  return v->visitArraySubscriptOp(this);
}
llvm::Value *FunctionCall::accept(Visitor<llvm::Value *> *v) {
  return v->visitFunctionCall(this);
}
llvm::Value *Unary::accept(Visitor<llvm::Value *> *v) {
  return v->visitUnary(this);
}
llvm::Value *SizeOf::accept(Visitor<llvm::Value *> *v) {
  return v->visitSizeOf(this);
}
llvm::Value *Binary::accept(Visitor<llvm::Value *> *v) {
  return v->visitBinary(this);
}
llvm::Value *Ternary::accept(Visitor<llvm::Value *> *v) {
  return v->visitTernary(this);
}
llvm::Value *Assignment::accept(Visitor<llvm::Value *> *v) {
  return v->visitAssignment(this);
}

} // namespace ccc
