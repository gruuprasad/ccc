#ifndef C4_GRAPHVIZ_VISITOR_HPP
#define C4_GRAPHVIZ_VISITOR_HPP
#include "../utils/utils.hpp"
#include "ast_node.hpp"

namespace ccc {

class GraphvizVisitor : public Visitor {
public:
  GraphvizVisitor() = default;
  ~GraphvizVisitor() = default;

  void visitTranslationUnit(TranslationUnit *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice((unsigned long)v, "TranslationUnit");
    for (const auto &child : v->extern_list)
      ss << Utils::makeGVEdge(
                (unsigned long)this,
                std::hash<std::unique_ptr<ExternalDeclaration>>()(child))
         << child->graphviz();
    std::cout << ss.str() << std::endl;
    //    return "subgraph cluster_" + std::to_string((unsigned long)v) + "{\n"
    //    +
    //           ss.str() + "}\n";
  }

  void visitFunctionDefinition(FunctionDefinition *v) override {}

  void visitFunctionDeclaration(FunctionDeclaration *v) override {}

  void visitDataDeclaration(DataDeclaration *v) override {}

  void visitStructDeclaration(StructDeclaration *v) override {}

  void visitParamDeclaration(ParamDeclaration *v) override {}

  void visitScalarType(ScalarType *v) override {}

  void visitStructType(StructType *v) override {}

  void visitDirectDeclarator(DirectDeclarator *v) override {}

  void visitAbstractDeclarator(AbstractDeclarator *v) override {}

  void visitPointerDeclarator(PointerDeclarator *v) override {}

  void visitFunctionDeclarator(FunctionDeclarator *v) override {}

  void visitCompoundStmt(CompoundStmt *v) override {}

  void visitIfElse(IfElse *v) override {}

  void visitLabel(Label *v) override {}

  void visitWhile(While *v) override {}

  void visitGoto(Goto *v) override {}

  void visitExpressionStmt(ExpressionStmt *v) override {}

  void visitBreak(Break *v) override {}

  void visitReturn(Return *v) override {}

  void visitContinue(Continue *v) override {}

  void visitVariableName(VariableName *v) override {}

  void visitNumber(Number *v) override {}

  void visitCharacter(Character *v) override {}

  void visitString(String *v) override {}

  void visitMemberAccessOp(MemberAccessOp *v) override {}

  void visitArraySubscriptOp(ArraySubscriptOp *v) override {}

  void visitFunctionCall(FunctionCall *v) override {}

  void visitUnary(Unary *v) override {}

  void visitSizeOf(SizeOf *v) override {}

  void visitBinary(Binary *v) override {}

  void visitTernary(Ternary *v) override {}

  void visitAssignment(Assignment *v) override {}
};

} // namespace ccc
#endif // C4_GRAPHVIZ_VISITOR_HPP
