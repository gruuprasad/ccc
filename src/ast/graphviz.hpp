#ifndef C4_GRAPHVIZ_VISITOR_HPP
#define C4_GRAPHVIZ_VISITOR_HPP
#include "../utils/utils.hpp"
#include "ast_node.hpp"

namespace ccc {

class GraphvizVisitor : public Visitor {
public:
  GraphvizVisitor() = default;
  ~GraphvizVisitor() override = default;

  std::string visitTranslationUnit(TranslationUnit *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "TranslationUnit");
    for (const auto &child : v->extern_list)
      ss << Utils::makeGVEdge(v->hash(), child->hash()) << child->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "FunctionDefinition");
    ss << Utils::makeGVEdge(v->hash(), v->return_type->hash())
       << v->return_type->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->fn_name->hash())
       << v->fn_name->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->fn_body->hash())
       << v->fn_body->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "FunctionDeclaration");
    ss << Utils::makeGVEdge(v->hash(), v->return_type->hash())
       << v->return_type->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->fn_name->hash());
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "DataDeclaration");
    ss << Utils::makeGVEdge(v->hash(), v->data_type->hash())
       << v->data_type->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->data_name->hash())
       << v->data_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "StructDeclaration");
    ss << Utils::makeGVEdge(v->hash(), v->struct_type->hash())
       << v->struct_type->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->struct_alias->hash())
       << v->struct_alias->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "ParamDeclaration");
    ss << Utils::makeGVEdge(v->hash(), v->param_type->hash())
       << v->param_type->accept(this);
    if (v->param_name)
      ss << Utils::makeGVEdge(v->hash(), v->param_name->hash())
         << v->param_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitScalarType(ScalarType *v) override {
    return Utils::makeGVVertice(v->hash(),
                                "ScalarType \"" + v->prettyPrint(0) + "\"");
  }

  std::string visitStructType(StructType *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(),
                               "StructType \"" + v->struct_name + "\"");
    for (const auto &p : v->member_list)
      ss << Utils::makeGVEdge(
                v->hash(), std::hash<std::unique_ptr<ExternalDeclaration>>()(p))
         << p->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitDirectDeclarator(DirectDeclarator *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "DirectDeclarator");
    ss << Utils::makeGVEdge(
              v->hash(),
              std::hash<std::unique_ptr<VariableName>>()(v->identifer))
       << v->identifer->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    return Utils::makeGVVertice(v->hash(), "AbstractDeclarator *" +
                                               std::to_string(v->pointerCount));
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(),
                               "PointerDeclarator *" +
                                   std::to_string(v->indirection_level));
    ss << Utils::makeGVEdge(v->hash(), v->identifier->hash())
       << v->identifier->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "FunctionDeclarator");
    ss << Utils::makeGVEdge(v->hash(), v->identifier->hash())
       << v->identifier->accept(this);
    for (const auto &p : v->param_list)
      ss << Utils::makeGVEdge(v->hash(),
                              std::hash<std::unique_ptr<ParamDeclaration>>()(p))
         << p->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->return_ptr->hash())
       << v->return_ptr->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "CompoundStmt");
    for (const auto &child : v->block_items)
      ss << Utils::makeGVEdge(v->hash(),
                              std::hash<std::unique_ptr<ASTNode>>()(child))
         << child->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitIfElse(IfElse *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "IfElse");
    ss << Utils::makeGVEdge(v->hash(), v->condition->hash())
       << v->condition->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->ifStmt->hash())
       << v->ifStmt->accept(this);
    if (v->elseStmt)
      ss << Utils::makeGVEdge(v->hash(), v->elseStmt->hash())
         << v->elseStmt->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitLabel(Label *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Label");
    ss << Utils::makeGVEdge(v->hash(), v->label_name->hash())
       << v->label_name->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->stmt->hash())
       << v->stmt->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitWhile(While *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "While");
    ss << Utils::makeGVEdge(v->hash(), v->predicate->hash())
       << v->predicate->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->block->hash())
       << v->block->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitGoto(Goto *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Goto");
    ss << Utils::makeGVEdge(v->hash(), v->label_name->hash())
       << v->label_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitExpressionStmt(ExpressionStmt *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "ExpressionStmt");
    ss << Utils::makeGVEdge(v->hash(), v->expr->hash())
       << v->expr->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitBreak(Break *v) override {
    return Utils::makeGVVertice(v->hash(), "Break");
  }

  std::string visitReturn(Return *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Return");
    if (v->expr) {
      ss << Utils::makeGVEdge(v->hash(), v->expr->hash())
         << v->expr->accept(this);
      return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" +
             ss.str() + "}\n";
    }
    return ss.str();
  }

  std::string visitContinue(Continue *v) override {
    return Utils::makeGVVertice(v->hash(), "Continue");
  }

  std::string visitVariableName(VariableName *v) override {
    return Utils::makeGVVertice(v->hash(), "VariableName \"" + v->name + "\"");
  }

  std::string visitNumber(Number *v) override {
    return Utils::makeGVVertice(
        v->hash(), "Number \"" + std::to_string(v->num_value) + "\"");
  }

  std::string visitCharacter(Character *v) override {
    return Utils::makeGVVertice(
        v->hash(), "Character \"" + std::to_string(v->char_value) + "\"");
  }

  std::string visitString(String *v) override {
    return Utils::makeGVVertice(v->hash(), "String \"" + v->str_value + "\"");
  }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "MemberAccessOp");
    ss << Utils::makeGVEdge(v->hash(), v->struct_name->hash())
       << v->struct_name->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->member_name->hash())
       << v->member_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "ArraySubscriptOp");
    ss << Utils::makeGVEdge(v->hash(), v->array_name->hash())
       << v->array_name->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->index_value->hash())
       << v->index_value->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "FunctionCall");
    ss << Utils::makeGVEdge(v->hash(), v->callee_name->hash())
       << v->callee_name->accept(this);
    for (const auto &p : v->callee_args)
      ss << Utils::makeGVEdge(v->hash(), p->hash()) << p->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitUnary(Unary *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Unary");
    ss << Utils::makeGVEdge(v->hash(), v->operand->hash())
       << v->operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitSizeOf(SizeOf *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "SizeOf");
    if (v->operand)
      ss << Utils::makeGVEdge(v->hash(), v->operand->hash())
         << v->operand->accept(this);
    else
      ss << Utils::makeGVEdge(v->hash(), v->type_name->hash())
         << v->type_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitBinary(Binary *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Binary");
    ss << Utils::makeGVEdge(v->hash(), v->left_operand->hash())
       << v->left_operand->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->right_operand->hash())
       << v->right_operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitTernary(Ternary *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Ternary");
    ss << Utils::makeGVEdge(v->hash(), v->predicate->hash())
       << v->predicate->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->left_branch->hash())
       << v->left_branch->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->right_branch->hash())
       << v->right_branch->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }

  std::string visitAssignment(Assignment *v) override {
    std::stringstream ss;
    ss << Utils::makeGVVertice(v->hash(), "Assignment");
    ss << Utils::makeGVEdge(v->hash(), v->left_operand->hash())
       << v->left_operand->accept(this);
    ss << Utils::makeGVEdge(v->hash(), v->right_operand->hash())
       << v->right_operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) + "{\n" + ss.str() +
           "}\n";
  }
};

} // namespace ccc
#endif // C4_GRAPHVIZ_VISITOR_HPP
