#include "ast_node.hpp"
#include "utils/utils.hpp"

#include <sstream>
#include <string>

namespace ccc {

static std::unordered_map<BinaryOpValue, std::string, EnumClassHash>
    BinaryOpValueToString{{BinaryOpValue::MULTIPLY, " * "},
                          {BinaryOpValue::ADD, " + "},
                          {BinaryOpValue::SUBTRACT, " - "},
                          {BinaryOpValue::LESS_THAN, " < "},
                          {BinaryOpValue::EQUAL, " == "},
                          {BinaryOpValue::NOT_EQUAL, " != "},
                          {BinaryOpValue::LOGICAL_AND, " && "},
                          {BinaryOpValue::LOGICAL_OR, " || "},
                          {BinaryOpValue::ASSIGN, " = "}};

std::string TranslationUnit::prettyPrint(int) { return std::string(); }

std::string FunctionDefinition::prettyPrint(int lvl) {
  return indent(lvl) + return_type->prettyPrint(0) + fn_name->prettyPrint(0) +
         fn_body->prettyPrint(0);
}

std::string FunctionDeclaration::prettyPrint(int lvl) {
  return indent(lvl) + return_type->prettyPrint(0) + fn_name->prettyPrint(0) +
         ";\n";
}

std::string DataDeclaration::prettyPrint(int lvl) {
  return indent(lvl) + data_type->prettyPrint(0) + " " +
         data_name->prettyPrint(0) + ";\n";
}

std::string StructDeclaration::prettyPrint(int) {
  return struct_type->prettyPrint(0) + " " + struct_alias->prettyPrint(0);
}

std::string ParamDeclaration::prettyPrint(int) {
  return param_type->prettyPrint(0) +
         (param_name ? " " + param_name->prettyPrint(0) : "");
}

std::string ScalarType::prettyPrint(int) {
  switch (type_kind) {
  case ScalarTypeValue::VOID:
    return "void";
  case ScalarTypeValue::CHAR:
    return "char";
  case ScalarTypeValue::INT:
    return "int";
  default:
    return error;
  };
}

std::string StructType::prettyPrint(int lvl) {
  if (member_list.empty()) {
    return "struct " + struct_name;
  }
  std::stringstream ss;
  for (const auto &member : member_list) {
    ss << member->prettyPrint(lvl + 1);
  }
  return "struct " + struct_name + "\n" + indent(lvl) + "{\n" + ss.str() +
         indent(lvl) + "}";
}

std::string DirectDeclarator::prettyPrint(int) {
  return identifer->prettyPrint(0);
}

std::string PointerDeclarator::prettyPrint(int) {
  std::string pre, post;
  for (int i = 0; i < indirection_level; i++) {
    pre += "(*";
    post += ")";
  }
  return pre + identifer->prettyPrint(0) + post;
}

std::string FunctionDeclarator::prettyPrint(int) {
  std::stringstream ss;
  for (const auto &p : param_list) {
    ss << p->prettyPrint(0);
    if (p != param_list.back())
      ss << ", ";
  }
  return "(" + identifer->prettyPrint(0) + "(" + ss.str() + "))";
}

std::string CompoundStmt::prettyPrintBlock(int lvl) {
  std::stringstream ss;
  for (const auto &stat : block_items)
    ss << stat->prettyPrint(lvl + 1);
  return "{\n" + ss.str() + indent(lvl) + "}";
}

std::string CompoundStmt::prettyPrint(int lvl) {
  return indent(lvl) + prettyPrintBlock(lvl) + "\n";
}

std::string CompoundStmt::prettyPrintInline(int lvl) {
  return " " + prettyPrintBlock(lvl - 1) + "\n";
}

std::string CompoundStmt::prettyPrintScopeIndent(int lvl) {
  return " " + prettyPrintBlock(lvl - 1) + " ";
}

std::string IfElse::prettyPrint(int lvl) {
  return indent(lvl) + "if (" + condition->prettyPrint(0) + ")" +
         (elseStmt ? ifStmt->prettyPrintScopeIndent(lvl + 1) + "else" +
                         elseStmt->prettyPrintInlineIf(lvl + 1)
                   : ifStmt->prettyPrintInline(lvl + 1));
}

std::string IfElse::prettyPrintInline(int lvl) {
  return "\n" + prettyPrint(lvl);
}

std::string IfElse::prettyPrintInlineIf(int lvl) {
  return " if (" + condition->prettyPrint(0) + ")" +
         (elseStmt ? ifStmt->prettyPrintScopeIndent(lvl) + "else" +
                         elseStmt->prettyPrintInlineIf(lvl)
                   : ifStmt->prettyPrintInline(lvl));
}

std::string Label::prettyPrint(int lvl) {
  return label_name->prettyPrint(0) + ":\n" + stmt->prettyPrint(lvl);
}

std::string While::prettyPrint(int lvl) {
  return indent(lvl) + "while (" + predicate->prettyPrint(0) + ")" +
         block->prettyPrintInline(lvl + 1);
}

std::string Goto::prettyPrint(int lvl) {
  return indent(lvl) + "goto " + label_name->prettyPrint(0) + ";\n";
}

std::string ExpressionStmt::prettyPrint(int lvl) {
  return indent(lvl) + expr->prettyPrint(0) + ";\n";
}

std::string Break::prettyPrint(int lvl) { return indent(lvl) + "break;\n"; }

std::string Return::prettyPrint(int lvl) {
  return indent(lvl) + "return" + (expr ? " " + expr->prettyPrint(0) : "") +
         ";\n";
}

std::string Continue::prettyPrint(int lvl) {
  return indent(lvl) + "continue;\n";
}

std::string VariableName::prettyPrint(int) { return name; }

std::string Number::prettyPrint(int) { return std::to_string(num_value); }

std::string Character::prettyPrint(int) {
  return "\'" + std::string(1, char_value) + "\'";
}

std::string String::prettyPrint(int) { return "\"" + str_value + "\""; }

std::string MemberAccessOp::prettyPrint(int) { return std::string(); }

std::string ArraySubscriptOp::prettyPrint(int) { return std::string(); }

std::string FunctionCall::prettyPrint(int) { return std::string(); }

std::string Unary::prettyPrint(int) {
  return "(" + getTokenRef().name() + operand->prettyPrint(0) + ")";
}

std::string SizeOf::prettyPrint(int) { return std::string(); }

std::string Binary::prettyPrint(int) {
  return "(" + left_operand->prettyPrint(0) + BinaryOpValueToString[op_kind] +
         right_operand->prettyPrint(0) + ")";
}

std::string Ternary::prettyPrint(int) {
  return "(" + predicate->prettyPrint(0) + " ? " + left_branch->prettyPrint(0) +
         " : " + right_branch->prettyPrint(0) + ")";
}

std::string Assignment::prettyPrint(int) { return std::string(); }

} // namespace ccc
