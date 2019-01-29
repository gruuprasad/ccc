#include "ast_node.hpp"

namespace ccc {

std::string TranslationUnit::accept(Visitor *v) {
  return v->visitTranslationUnit(this);
}
std::string FunctionDefinition::accept(Visitor *v) {
  return v->visitFunctionDefinition(this);
}

std::string FunctionDeclaration::accept(Visitor *v) {
  return v->visitFunctionDeclaration(this);
}

std::string DataDeclaration::accept(Visitor *v) {
  return v->visitDataDeclaration(this);
}

std::string StructDeclaration::accept(Visitor *v) {
  return v->visitStructDeclaration(this);
}
std::string ParamDeclaration::accept(Visitor *v) {
  return v->visitParamDeclaration(this);
}

std::string ScalarType::accept(Visitor *v) { return v->visitScalarType(this); }
std::string StructType::accept(Visitor *v) { return v->visitStructType(this); }
std::string DirectDeclarator::accept(Visitor *v) {
  return v->visitDirectDeclarator(this);
}
std::string AbstractDeclarator::accept(Visitor *v) {
  return v->visitAbstractDeclarator(this);
}
std::string PointerDeclarator::accept(Visitor *v) {
  return v->visitPointerDeclarator(this);
}
std::string FunctionDeclarator::accept(Visitor *v) {
  return v->visitFunctionDeclarator(this);
}
std::string CompoundStmt::accept(Visitor *v) {
  return v->visitCompoundStmt(this);
}
std::string IfElse::accept(Visitor *v) { return v->visitIfElse(this); }
std::string Label::accept(Visitor *v) { return v->visitLabel(this); }
std::string While::accept(Visitor *v) { return v->visitWhile(this); }
std::string Goto::accept(Visitor *v) { return v->visitGoto(this); }
std::string ExpressionStmt::accept(Visitor *v) {
  return v->visitExpressionStmt(this);
}
std::string Break::accept(Visitor *v) { return v->visitBreak(this); }
std::string Return::accept(Visitor *v) { return v->visitReturn(this); }
std::string Continue::accept(Visitor *v) { return v->visitContinue(this); }
std::string VariableName::accept(Visitor *v) {
  return v->visitVariableName(this);
}
std::string Number::accept(Visitor *v) { return v->visitNumber(this); }
std::string Character::accept(Visitor *v) { return v->visitCharacter(this); }
std::string String::accept(Visitor *v) { return v->visitString(this); }
std::string MemberAccessOp::accept(Visitor *v) {
  return v->visitMemberAccessOp(this);
}
std::string ArraySubscriptOp::accept(Visitor *v) {
  return v->visitArraySubscriptOp(this);
}
std::string FunctionCall::accept(Visitor *v) {
  return v->visitFunctionCall(this);
}
std::string Unary::accept(Visitor *v) { return v->visitUnary(this); }
std::string SizeOf::accept(Visitor *v) { return v->visitSizeOf(this); }
std::string Binary::accept(Visitor *v) { return v->visitBinary(this); }
std::string Ternary::accept(Visitor *v) { return v->visitTernary(this); }
std::string Assignment::accept(Visitor *v) { return v->visitAssignment(this); }

} // namespace ccc
