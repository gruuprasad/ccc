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

void TranslationUnit::accept(Visitor<void> *v) {
  return v->visitTranslationUnit(this);
}

void FunctionDefinition::accept(Visitor<void> *v) {
  return v->visitFunctionDefinition(this);
}

void FunctionDeclaration::accept(Visitor<void> *v) {
  return v->visitFunctionDeclaration(this);
}

void DataDeclaration::accept(Visitor<void> *v) {
  return v->visitDataDeclaration(this);
}

void StructDeclaration::accept(Visitor<void> *v) {
  return v->visitStructDeclaration(this);
}

void ParamDeclaration::accept(Visitor<void> *v) {
  return v->visitParamDeclaration(this);
}

void ScalarType::accept(Visitor<void> *v) { return v->visitScalarType(this); }

void StructType::accept(Visitor<void> *v) { return v->visitStructType(this); }

void AbstractType::accept(Visitor<void> *v) {
  return v->visitAbstractType(this);
}

void DirectDeclarator::accept(Visitor<void> *v) {
  return v->visitDirectDeclarator(this);
}

void AbstractDeclarator::accept(Visitor<void> *v) {
  return v->visitAbstractDeclarator(this);
}

void PointerDeclarator::accept(Visitor<void> *v) {
  return v->visitPointerDeclarator(this);
}

void FunctionDeclarator::accept(Visitor<void> *v) {
  return v->visitFunctionDeclarator(this);
}

void CompoundStmt::accept(Visitor<void> *v) {
  return v->visitCompoundStmt(this);
}

void IfElse::accept(Visitor<void> *v) { return v->visitIfElse(this); }

void Label::accept(Visitor<void> *v) { return v->visitLabel(this); }

void While::accept(Visitor<void> *v) { return v->visitWhile(this); }

void Goto::accept(Visitor<void> *v) { return v->visitGoto(this); }

void ExpressionStmt::accept(Visitor<void> *v) {
  return v->visitExpressionStmt(this);
}

void Break::accept(Visitor<void> *v) { return v->visitBreak(this); }

void Return::accept(Visitor<void> *v) { return v->visitReturn(this); }

void Continue::accept(Visitor<void> *v) { return v->visitContinue(this); }

void VariableName::accept(Visitor<void> *v) {
  return v->visitVariableName(this);
}

void Number::accept(Visitor<void> *v) { return v->visitNumber(this); }

void Character::accept(Visitor<void> *v) { return v->visitCharacter(this); }

void String::accept(Visitor<void> *v) { return v->visitString(this); }

void MemberAccessOp::accept(Visitor<void> *v) {
  return v->visitMemberAccessOp(this);
}

void ArraySubscriptOp::accept(Visitor<void> *v) {
  return v->visitArraySubscriptOp(this);
}

void FunctionCall::accept(Visitor<void> *v) {
  return v->visitFunctionCall(this);
}

void Unary::accept(Visitor<void> *v) { return v->visitUnary(this); }

void SizeOf::accept(Visitor<void> *v) { return v->visitSizeOf(this); }

void Binary::accept(Visitor<void> *v) { return v->visitBinary(this); }

void Ternary::accept(Visitor<void> *v) { return v->visitTernary(this); }

void Assignment::accept(Visitor<void> *v) { return v->visitAssignment(this); }
} // namespace ccc
