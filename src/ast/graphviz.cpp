#if GRAPHVIZ
#include "ast_node.hpp"
#include "utils/utils.hpp"

namespace ccc {

std::string ASTNode::toGraph() {
  return "graph ast "
         "{\nsplines=line;\nstyle=dotted;\nsubgraph "
         "cluster{\n" +
         this->graphviz() + "}\n}\n";
}

std::string TranslationUnit::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "TranslationUnit");
  for (const auto &child : this->extern_list)
    ss << Utils::makeGVEdge(
              (unsigned long)this,
              std::hash<std::unique_ptr<ExternalDeclaration>>()(child))
       << child->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string FunctionDefinition::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "FunctionDefinition");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Type>>()(return_type))
     << return_type->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(fn_name))
     << fn_name->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Statement>>()(fn_body))
     << fn_body->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string FunctionDeclaration::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "FunctionDeclaration");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Type>>()(return_type))
     << return_type->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(fn_name))
     << fn_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string DataDeclaration::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "DataDeclaration");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Type>>()(data_type))
     << data_type->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(data_name))
     << data_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string StructDeclaration::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "StructDeclaration");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Type>>()(struct_type))
     << struct_type->graphviz();
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Declarator>>()(struct_alias))
     << struct_alias->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string ParamDeclaration::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "ParamDeclaration");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Type>>()(param_type))
     << param_type->graphviz();
  if (param_name)
    ss << Utils::makeGVEdge(
              (unsigned long)this,
              std::hash<std::unique_ptr<Declarator>>()(param_name))
       << param_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string ScalarType::graphviz() {
  return Utils::makeGVVertice((unsigned long)this,
                              "ScalarType \"" + prettyPrint(0) + "\"");
}

std::string StructType::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this,
                             "StructType \"" + struct_name + "\"");
  for (const auto &p : member_list)
    ss << Utils::makeGVEdge(
              (unsigned long)this,
              std::hash<std::unique_ptr<ExternalDeclaration>>()(p))
       << p->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string DirectDeclarator::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "DirectDeclarator");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<VariableName>>()(identifer))
     << identifer->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string AbstractDeclarator::graphviz() {
  return Utils::makeGVVertice((unsigned long)this,
                              "AbstractDeclarator *" +
                                  std::to_string(pointerCount));
}

std::string PointerDeclarator::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this,
                             "PointerDeclarator *" +
                                 std::to_string(indirection_level));
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(identifer))
     << identifer->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string FunctionDeclarator::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "FunctionDeclarator");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(identifer))
     << identifer->graphviz();
  for (const auto &p : param_list)
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<ParamDeclaration>>()(p))
       << p->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Declarator>>()(return_ptr))
     << return_ptr->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string CompoundStmt::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "CompoundStmt");
  for (const auto &child : block_items)
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<ASTNode>>()(child))
       << child->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string IfElse::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "IfElse");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(condition))
     << condition->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Statement>>()(ifStmt))
     << ifStmt->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Statement>>()(elseStmt))
     << elseStmt->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Label::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Label");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(label_name))
     << label_name->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Statement>>()(stmt))
     << stmt->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string While::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "While");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(predicate))
     << predicate->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Statement>>()(block))
     << block->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Goto::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Goto");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(label_name))
     << label_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string ExpressionStmt::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "ExpressionStmt");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(expr))
     << expr->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Break::graphviz() {
  return Utils::makeGVVertice((unsigned long)this, "Break");
}

std::string Return::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Return");
  if (expr) {
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<Expression>>()(expr))
       << expr->graphviz();
    return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
           ss.str() + "}\n";
  }
  return ss.str();
}

std::string Continue::graphviz() {
  return Utils::makeGVVertice((unsigned long)this, "Continue");
}

std::string VariableName::graphviz() {
  return Utils::makeGVVertice((unsigned long)this,
                              "VariableName \"" + name + "\"");
}

std::string Number::graphviz() {
  return Utils::makeGVVertice((unsigned long)this,
                              "Number \"" + std::to_string(num_value) + "\"");
}

std::string Character::graphviz() {
  return Utils::makeGVVertice(
      (unsigned long)this, "Character \"" + std::to_string(char_value) + "\"");
}

std::string String::graphviz() {
  return Utils::makeGVVertice((unsigned long)this,
                              "String \"" + str_value + "\"");
}

std::string MemberAccessOp::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "MemberAccessOp");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(struct_name))
     << struct_name->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(member_name))
     << member_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string ArraySubscriptOp::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "ArraySubscriptOp");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(array_name))
     << array_name->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(index_value))
     << index_value->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string FunctionCall::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "FunctionCall");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(callee_name))
     << callee_name->graphviz();
  for (const auto &p : callee_args)
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<Expression>>()(p))
       << p->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Unary::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Unary");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(operand))
     << operand->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string SizeOf::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "SizeOf");
  if (operand)
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<Expression>>()(operand))
       << operand->graphviz();
  else
    ss << Utils::makeGVEdge((unsigned long)this,
                            std::hash<std::unique_ptr<Type>>()(type_name))
       << type_name->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Binary::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Binary");
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Expression>>()(left_operand))
     << left_operand->graphviz();
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Expression>>()(right_operand))
     << right_operand->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Ternary::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Ternary");
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(predicate))
     << predicate->graphviz();
  ss << Utils::makeGVEdge((unsigned long)this,
                          std::hash<std::unique_ptr<Expression>>()(left_branch))
     << left_branch->graphviz();
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Expression>>()(right_branch))
     << right_branch->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

std::string Assignment::graphviz() {
  std::stringstream ss;
  ss << Utils::makeGVVertice((unsigned long)this, "Assignment");
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Expression>>()(left_operand))
     << left_operand->graphviz();
  ss << Utils::makeGVEdge(
            (unsigned long)this,
            std::hash<std::unique_ptr<Expression>>()(right_operand))
     << right_operand->graphviz();
  return "subgraph cluster_" + std::to_string((unsigned long)this) + "{\n" +
         ss.str() + "}\n";
}

} // namespace ccc
#endif
