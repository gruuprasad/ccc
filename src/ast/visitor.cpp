#include "ast_node.hpp"

namespace ccc {

void TranslationUnit::accept(Visitor *v) { v->visitTranslationUnit(this); }
void FunctionDefinition::accept(Visitor *v) {
  v->visitFunctionDefinition(this);
}
void FunctionDeclaration::accept(Visitor *v) {
  v->visitFunctionDeclaration(this);
}
void DataDeclaration::accept(Visitor *v) { v->visitDataDeclaration(this); }
void StructDeclaration::accept(Visitor *v) { v->visitStructDeclaration(this); }
void ParamDeclaration::accept(Visitor *v) { v->visitParamDeclaration(this); }
void ScalarType::accept(Visitor *v) { v->visitScalarType(this); }
void StructType::accept(Visitor *v) { v->visitStructType(this); }
void DirectDeclarator::accept(Visitor *v) { v->visitDirectDeclarator(this); }
void AbstractDeclarator::accept(Visitor *v) {
  v->visitAbstractDeclarator(this);
}
void PointerDeclarator::accept(Visitor *v) { v->visitPointerDeclarator(this); }
void FunctionDeclarator::accept(Visitor *v) {
  v->visitFunctionDeclarator(this);
}
void CompoundStmt::accept(Visitor *v) { v->visitCompoundStmt(this); }
void IfElse::accept(Visitor *v) { v->visitIfElse(this); }
void Label::accept(Visitor *v) { v->visitLabel(this); }
void While::accept(Visitor *v) { v->visitWhile(this); }
void Goto::accept(Visitor *v) { v->visitGoto(this); }
void ExpressionStmt::accept(Visitor *v) { v->visitExpressionStmt(this); }
void Break::accept(Visitor *v) { v->visitBreak(this); }
void Return::accept(Visitor *v) { v->visitReturn(this); }
void Continue::accept(Visitor *v) { v->visitContinue(this); }
void VariableName::accept(Visitor *v) { v->visitVariableName(this); }
void Number::accept(Visitor *v) { v->visitNumber(this); }
void Character::accept(Visitor *v) { v->visitCharacter(this); }
void String::accept(Visitor *v) { v->visitString(this); }
void MemberAccessOp::accept(Visitor *v) { v->visitMemberAccessOp(this); }
void ArraySubscriptOp::accept(Visitor *v) { v->visitArraySubscriptOp(this); }
void FunctionCall::accept(Visitor *v) { v->visitFunctionCall(this); }
void Unary::accept(Visitor *v) { v->visitUnary(this); }
void SizeOf::accept(Visitor *v) { v->visitSizeOf(this); }
void Binary::accept(Visitor *v) { v->visitBinary(this); }
void Ternary::accept(Visitor *v) { v->visitTernary(this); }
void Assignment::accept(Visitor *v) { v->visitAssignment(this); }

} // namespace ccc
