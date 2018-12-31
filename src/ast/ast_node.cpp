#include "ast_node.hpp"

#include <sstream>

namespace ccc {

std::string BinaryExpression::prettyPrint(int) {
  return "(" + this->leftExpr->prettyPrint(0) + " " + this->op + " " +
         this->rightExpr->prettyPrint(0) + ")";
}

std::string BreakStatement::prettyPrint(int lvl) {
  return indent(lvl) + "break;\n";
}

std::string CallExpression::prettyPrint(int) {
  std::stringstream ss;
  ss << "(" << call->prettyPrint(0) << "(";
  for (Expression *arg : args) {
    ss << arg->prettyPrint(0);
    if (arg != args.back())
      ss << ", ";
  }
  ss << "))";
  return ss.str();
}
std::string CompoundStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl + 1);
  }
  return indent(lvl) + "{\n" + ss.str() + indent(lvl) + "}\n";
}

std::string CompoundStatement::prettyPrintBlock(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "}\n";
}

std::string CompoundStatement::prettyPrintScopeIndent(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "} ";
}

std::string CompoundStatement::prettyPrintStruct(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl + 1);
  }
  return "{\n" + ss.str() + indent(lvl) + "} ";
}

std::string ContinueStatement::prettyPrint(int lvl) {
  return indent(lvl) + "continue;\n";
}

std::string DeclarationStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << indent(lvl) << type->prettyPrint(0);
  if (body)
    ss << "\n" << body->prettyPrint(lvl);
  else
    ss << ";\n";
  return ss.str();
}

std::string ExpressionStatement::prettyPrint(int lvl) {
  if (this->expr)
    return indent(lvl) + expr->prettyPrint(0) + ";\n";
  else
    return indent(lvl) + ";\n";
}

std::string GotoStatement::prettyPrint(int lvl) {
  return indent(lvl) + "goto " + this->expr->prettyPrint(0) + ";\n";
}

std::string IfElseStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  if (this->elseStat) {
    ss << indent(lvl) << "if (" << this->expr->prettyPrint(0) << ")"
       << this->ifStat->prettyPrintScopeIndent(lvl + 1);
    ss << "else" << this->elseStat->prettyPrintInlineIf(lvl + 1);
  } else {
    ss << indent(lvl) << "if (" << this->expr->prettyPrint(0) << ")"
       << this->ifStat->prettyPrintBlock(lvl + 1);
  }
  return ss.str();
}

std::string IfElseStatement::prettyPrintBlock(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string IfElseStatement::prettyPrintInlineIf(int lvl) {
  std::stringstream ss;
  if (this->elseStat) {
    ss << " if (" << this->expr->prettyPrint(0) << ")"
       << this->ifStat->prettyPrintScopeIndent(lvl);
    ss << "else" << this->elseStat->prettyPrintBlock(lvl);
  } else {
    ss << " if (" << this->expr->prettyPrint(0) << ")"
       << this->ifStat->prettyPrintBlock(lvl);
  }
  return ss.str();
}

std::string LabeledStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << this->expr->prettyPrint(0) + ":\n";
  if (this->stat)
    ss << this->stat->prettyPrint(lvl);
  return ss.str();
}

std::string ReturnStatement::prettyPrint(int lvl) {
  if (expr)
    return indent(lvl) + "return " + expr->prettyPrint(0) + ";\n";
  else
    return indent(lvl) + "return;\n";
}

std::string StructStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << indent(lvl) << "struct " << this->name->prettyPrint(0) << "\n";
  ss << indent(lvl) << body->prettyPrintStruct(lvl);
  ss << this->alias->prettyPrint(0) << ";\n";
  return ss.str();
}

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  for (ASTNode *node : this->children)
    ss << node->prettyPrint(lvl);
  return ss.str();
}

std::string TypeExpression::prettyPrint(int) {
  std::stringstream ss;
  switch (baseType) {
  case TypeSpec::VOID:
    ss << "void";
    break;
  case TypeSpec::CHAR:
    ss << "char";
    break;
  case TypeSpec::INT:
    ss << "int";
    break;
  case TypeSpec::POINTER:
    ss << "*";
    if (expr) {
      ss << "(" << expr->prettyPrint(0) << ")";
    } else if (iden) {
      ss << iden->prettyPrint(0);
    }
    return ss.str();
  case TypeSpec::STRUCT:
    ss << "struct ";
    ss << iden->prettyPrint(0);
    if (expr) {
      ss << " (" << expr->prettyPrint(0) << ")";
    }
    return ss.str();
  case TypeSpec::FUNCTION:
    if (expr)
      ss << "(" << expr->prettyPrint(0) << ")(";
    else if (iden)
      ss << iden->prettyPrint(0) << "(";
    for (TypeExpression *arg : args) {
      ss << arg->prettyPrint(0);
      if (arg != args.back())
        ss << ", ";
    }
    ss << ")";
    return ss.str();
  }
  if (expr) {
    ss << " (" << expr->prettyPrint(0) << ")";
  } else if (iden) {
    ss << " " << iden->prettyPrint(0);
  }
  return ss.str();
}

std::string WhileStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << indent(lvl) << "while (" << this->expr->prettyPrint(0) << ")"
     << this->stat->prettyPrintBlock(lvl + 1);
  return ss.str();
}

} // namespace ccc
