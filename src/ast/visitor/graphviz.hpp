#ifndef C4_GRAPHVIZ_VISITOR_HPP
#define C4_GRAPHVIZ_VISITOR_HPP
#include "../../utils/utils.hpp"
#include "../ast_node.hpp"
#include "pretty_printer.hpp"

namespace ccc {
/**
 * AST visitor class to generate graphviz code of tree as string
 */
class GraphvizVisitor : public Visitor<std::string> {
  /**
   * generate blue node
   *
   * @param hash
   * @param name
   * @return string
   */
  static std::string makeGVVertice(unsigned long hash,
                                   const std::string &name) {
    std::stringstream ss;
    ss << hash
       << "[label=<" + name +
              "> shape=ellipse style=filled "
              "fillcolor=lightskyblue];\n";
    return ss.str();
  }

  /**
   * generate yellow node
   *
   * @param hash
   * @param name
   * @return string
   */
  static std::string makeGVVerticeExpr(unsigned long hash,
                                       const std::string &name) {
    std::stringstream ss;
    ss << hash
       << "[label=<" + name +
              "> shape=ellipse style=filled "
              "fillcolor=khaki];\n";
    return ss.str();
  }

  /**
   * generate grey node
   *
   * @param hash
   * @param name
   * @return string
   */
  static std::string makeGVVerticeBox(unsigned long hash,
                                      const std::string &name) {
    std::stringstream ss;
    ss << hash
       << "[label=<" + name +
              "> shape=box style=filled "
              "fillcolor=lightgrey];\n";
    return ss.str();
  }

  PrettyPrinterVisitor pp;

  /**
   * connect two nodes with an edge
   *
   * @param left
   * @param right
   * @return string
   */
  static std::string makeGVEdge(unsigned long left, unsigned long right) {
    std::stringstream ss;
    ss << left << "--" << right << ";\n";
    return ss.str();
  }

public:
  GraphvizVisitor() = default;
  ~GraphvizVisitor() override = default;

  /**
   * root of AST, return graphziv header
   *
   * @param v visitor
   * @return
   */
  std::string visitTranslationUnit(TranslationUnit *v) override {
    std::stringstream ss;
    ss << "graph ast{\nsplines=line;\nstyle=invis;\nsubgraph cluster{\n";
    ss << makeGVVertice(v->hash(), "TranslationUnit");
    ss << "subgraph cluster_" << std::to_string(v->hash())
       << "{\nstyle=dotted\n";
    for (const auto &child : v->extern_list)
      ss << makeGVEdge(v->hash(), child->hash()) << child->accept(this);
    ss << "}\n}\n}\n";
    return ss.str();
  }

  /**
   * create unique node (using hash) and connect with children - all methods
   * work the same from here on
   *
   * @param v visitor
   * @return string
   */
  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "FunctionDefinition");
    ss << makeGVEdge(v->hash(), v->return_type->hash())
       << v->return_type->accept(this);
    ss << makeGVEdge(v->hash(), v->fn_name->hash()) << v->fn_name->accept(this);
    ss << makeGVEdge(v->hash(), v->fn_body->hash()) << v->fn_body->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "FunctionDeclaration");
    ss << makeGVEdge(v->hash(), v->return_type->hash())
       << v->return_type->accept(this);
    if (v->fn_name)
      ss << makeGVEdge(v->hash(), v->fn_name->hash())
         << v->fn_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "DataDeclaration");
    ss << makeGVEdge(v->hash(), v->data_type->hash())
       << v->data_type->accept(this);
    if (v->data_name)
      ss << makeGVEdge(v->hash(), v->data_name->hash())
         << v->data_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "StructDeclaration");
    ss << makeGVEdge(v->hash(), v->struct_type->hash())
       << v->struct_type->accept(this);
    if (v->struct_alias)
      ss << makeGVEdge(v->hash(), v->struct_alias->hash())
         << v->struct_alias->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "ParamDeclaration");
    ss << makeGVEdge(v->hash(), v->param_type->hash())
       << v->param_type->accept(this);
    if (v->param_name)
      ss << makeGVEdge(v->hash(), v->param_name->hash())
         << v->param_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitScalarType(ScalarType *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVerticeBox(v->hash(),
                            "ScalarType \"" + v->accept(&pp) + "\"") +
           "}\n";
  }

  std::string visitAbstractType(AbstractType *v) override {
    std::stringstream ss;
    ss << makeGVVerticeBox(v->hash(), "AbstractType");
    ss << makeGVEdge(v->hash(), v->type->hash()) << v->type->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitStructType(StructType *v) override {
    std::stringstream ss;
    if (v->struct_name)
      ss << makeGVVerticeBox(v->hash(),
                             "StructType \"" + v->struct_name->name + "\"");
    else
      ss << makeGVVerticeBox(v->hash(), "StructType");
    for (const auto &p : v->member_list)
      ss << makeGVEdge(v->hash(),
                       std::hash<std::unique_ptr<ExternalDeclaration>>()(p))
         << p->accept(this);
    if (v->is_definition)
      return "subgraph cluster_" + std::to_string(v->hash()) +
             "{\nstyle=dotted;\n" + ss.str() + "}\n";
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitDirectDeclarator(DirectDeclarator *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "DirectDeclarator");
    ss << makeGVEdge(v->hash(),
                     std::hash<std::unique_ptr<VariableName>>()(v->identifer))
       << v->identifer->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVertice(v->hash(), "AbstractDeclarator *" +
                                        std::to_string(v->pointerCount)) +
           "}\n";
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "PointerDeclarator *" +
                                       std::to_string(v->indirection_level));
    ss << makeGVEdge(v->hash(), v->identifier->hash())
       << v->identifier->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "FunctionDeclarator");
    ss << makeGVEdge(v->hash(), v->identifier->hash())
       << v->identifier->accept(this);
    for (const auto &p : v->param_list)
      ss << makeGVEdge(v->hash(),
                       std::hash<std::unique_ptr<ParamDeclaration>>()(p))
         << p->accept(this);
    ss << makeGVEdge(v->hash(), v->return_ptr->hash())
       << v->return_ptr->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "CompoundStmt");
    for (const auto &child : v->block_items)
      ss << makeGVEdge(v->hash(), std::hash<std::unique_ptr<ASTNode>>()(child))
         << child->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=dotted;\n" + ss.str() + "}\n";
  }

  std::string visitIfElse(IfElse *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "IfElse");
    ss << makeGVEdge(v->hash(), v->condition->hash())
       << v->condition->accept(this);
    ss << makeGVEdge(v->hash(), v->ifStmt->hash()) << v->ifStmt->accept(this);
    if (v->elseStmt)
      ss << makeGVEdge(v->hash(), v->elseStmt->hash())
         << v->elseStmt->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitLabel(Label *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "Label");
    ss << makeGVEdge(v->hash(), v->label_name->hash())
       << v->label_name->accept(this);
    ss << makeGVEdge(v->hash(), v->stmt->hash()) << v->stmt->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitWhile(While *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "While");
    ss << makeGVEdge(v->hash(), v->predicate->hash())
       << v->predicate->accept(this);
    ss << makeGVEdge(v->hash(), v->block->hash()) << v->block->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitGoto(Goto *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "Goto");
    ss << makeGVEdge(v->hash(), v->label_name->hash())
       << v->label_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitExpressionStmt(ExpressionStmt *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "ExpressionStmt");
    if (v->expr)
      ss << makeGVEdge(v->hash(), v->expr->hash()) << v->expr->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitBreak(Break *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + makeGVVertice(v->hash(), "Break") + "}\n";
  }

  std::string visitReturn(Return *v) override {
    std::stringstream ss;
    ss << makeGVVertice(v->hash(), "Return");
    if (v->expr) {
      ss << makeGVEdge(v->hash(), v->expr->hash()) << v->expr->accept(this);
      return "subgraph cluster_" + std::to_string(v->hash()) +
             "{\nstyle=invis;\n" + ss.str() + "}\n";
    }
    return ss.str();
  }

  std::string visitContinue(Continue *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + makeGVVertice(v->hash(), "Continue") + "}\n";
  }

  std::string visitVariableName(VariableName *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVerticeBox(v->hash(), "VariableName \"" + v->name + "\"") +
           "}\n";
  }

  std::string visitNumber(Number *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVerticeBox(v->hash(),
                            "Number \"" + std::to_string(v->num_value) + "\"") +
           "}\n";
  }

  std::string visitCharacter(Character *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVerticeBox(v->hash(), "Character \"" + v->char_value + "\"") +
           "}\n";
  }

  std::string visitString(String *v) override {
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" +
           makeGVVerticeBox(v->hash(), "String \"" + v->str_value + "\"") +
           "}\n";
  }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "MemberAccessOp");
    ss << makeGVEdge(v->hash(), v->struct_name->hash())
       << v->struct_name->accept(this);
    ss << makeGVEdge(v->hash(), v->member_name->hash())
       << v->member_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "ArraySubscriptOp");
    ss << makeGVEdge(v->hash(), v->array_name->hash())
       << v->array_name->accept(this);
    ss << makeGVEdge(v->hash(), v->index_value->hash())
       << v->index_value->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "FunctionCall");
    ss << makeGVEdge(v->hash(), v->callee_name->hash())
       << v->callee_name->accept(this);
    for (const auto &p : v->callee_args)
      ss << makeGVEdge(v->hash(), p->hash()) << p->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitUnary(Unary *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "Unary");
    ss << makeGVEdge(v->hash(), v->operand->hash()) << v->operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitSizeOf(SizeOf *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "SizeOf");
    if (v->operand)
      ss << makeGVEdge(v->hash(), v->operand->hash())
         << v->operand->accept(this);
    else
      ss << makeGVEdge(v->hash(), v->type_name->hash())
         << v->type_name->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitBinary(Binary *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "Binary");
    ss << makeGVEdge(v->hash(), v->left_operand->hash())
       << v->left_operand->accept(this);
    ss << makeGVEdge(v->hash(), v->right_operand->hash())
       << v->right_operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitTernary(Ternary *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "Ternary");
    ss << makeGVEdge(v->hash(), v->predicate->hash())
       << v->predicate->accept(this);
    ss << makeGVEdge(v->hash(), v->left_branch->hash())
       << v->left_branch->accept(this);
    ss << makeGVEdge(v->hash(), v->right_branch->hash())
       << v->right_branch->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }

  std::string visitAssignment(Assignment *v) override {
    std::stringstream ss;
    ss << makeGVVerticeExpr(v->hash(), "Assignment");
    ss << makeGVEdge(v->hash(), v->left_operand->hash())
       << v->left_operand->accept(this);
    ss << makeGVEdge(v->hash(), v->right_operand->hash())
       << v->right_operand->accept(this);
    return "subgraph cluster_" + std::to_string(v->hash()) +
           "{\nstyle=invis;\n" + ss.str() + "}\n";
  }
};
} // namespace ccc

#endif // C4_GRAPHVIZ_VISITOR_HPP
