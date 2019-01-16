#include "ast_node.hpp"

#include <sstream>
#include <string>

namespace ccc {

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

std::string CompoundStmt::prettyPrint(int lvl) {
  std::stringstream ss;
  for (const auto &stat : block_items)
    ss << stat->prettyPrint(lvl + 1);
  return indent(lvl) + "{\n" + ss.str() + indent(lvl) + "}\n";
}

std::string IfElse::prettyPrint(int lvl) {
  return indent(lvl) + "if (" + condition->prettyPrint(0) + ")\n" +
         (elseStmt ? ifStmt->prettyPrint(lvl + 1) + "else" +
                         elseStmt->prettyPrint(lvl + 1)
                   : ifStmt->prettyPrint(lvl + 1));
}

std::string Label::prettyPrint(int) { return std::string(); }

std::string While::prettyPrint(int) { return std::string(); }

std::string Goto::prettyPrint(int) { return std::string(); }

std::string ExpressionStmt::prettyPrint(int lvl) {
  return indent(lvl) + (expr ? expr->prettyPrint(0) : "") + ";\n";
}

std::string Break::prettyPrint(int) { return std::string(); }

std::string Return::prettyPrint(int) { return std::string(); }

std::string Continue::prettyPrint(int) { return std::string(); }

std::string VariableName::prettyPrint(int) { return name; }

std::string Number::prettyPrint(int) { return std::to_string(num_value); }

std::string Character::prettyPrint(int) { return std::to_string(char_value); }

std::string String::prettyPrint(int) { return str_value; }

std::string MemberAccessOp::prettyPrint(int) { return std::string(); }

std::string ArraySubscriptOp::prettyPrint(int) { return std::string(); }

std::string FunctionCall::prettyPrint(int) { return std::string(); }

std::string Unary::prettyPrint(int) {
  return "(" + getTokenRef().name() + operand->prettyPrint(0) + ")";
}

std::string SizeOf::prettyPrint(int) { return std::string(); }

std::string Binary::prettyPrint(int) {
  return "(" + left_operand->prettyPrint(0) + " " + getTokenRef().name() + " " +
         right_operand->prettyPrint(0) + ")";
}

std::string Ternary::prettyPrint(int) { return std::string(); }

std::string Assignment::prettyPrint(int) { return std::string(); }

std::string Statement::indent(int n) {
  if (n >= 0) {
    std::stringstream ss;
    for (int i = 0; i < n; i++)
      ss << "\t";
    return ss.str();
  } else
    return "";
}
} // namespace ccc
