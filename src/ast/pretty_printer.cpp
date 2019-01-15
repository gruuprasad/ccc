#include "ast_node.hpp"

#include <sstream>
#include <string>

namespace ccc {

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  if (extern_list.empty()) {
    return ss.str();
  }

  auto it = extern_list.cbegin();
  auto last_node = std::prev(extern_list.cend());

  while (it != last_node) {
    ss << (*it)->prettyPrint(lvl) << "\n";
    ++it;
  }
  ss << (*it)->prettyPrint(lvl);

  return ss.str();
}

std::string FunctionDefinition::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string FunctionDeclaration::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string DataDeclaration::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string StructDeclaration::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string ParamDeclaration::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string ScalarType::prettyPrint(int) {
  std::stringstream ss;
  switch (type_kind) {
  case ScalarTypeValue::VOID:
    ss << "void ";
    break;
  case ScalarTypeValue::CHAR:
    ss << "char ";
    break;
  case ScalarTypeValue::INT:
    ss << "int ";
    break;
  };
  return ss.str();
}

std::string StructType::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << "struct ";
  ss << struct_name << " ";

  if (member_list.empty()) {
    return ss.str();
  }

  // Go to next line and match current indent level
  ss << "\n" << std::string(lvl, '\t');
  ss << "{";
  lvl++;

  for (const auto &member : member_list) {
    ss << "\n" << std::string(lvl, '\t');
    member->prettyPrint(lvl);
  }
  lvl--;
  ss << "\n" << std::string(lvl, '\t');
  ss << "} ";

  return ss.str();
}

std::string DirectDeclarator::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string PointerDeclarator::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string FunctionDeclarator::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string CompoundStmt::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string ExpressionStmt::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string VariableName::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string Number::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string Character::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string String::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

std::string Binary::prettyPrint(int) {
  std::stringstream ss;

  return ss.str();
}

} // namespace ccc
