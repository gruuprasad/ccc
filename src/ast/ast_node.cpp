#include "ast_node.hpp"

#include <sstream>

namespace ccc {

// Methods to generate prettyprinting for different AST types.
std::string BinaryExpression::prettyPrint() {
  return "(" + children[0]->prettyPrint() + " " + token->name() + " " +
         children[1]->prettyPrint() + ")";
}

std::string Statement::indent(int n) {
  if (n >= 0) {
    std::stringstream ss;
    for (int i = 0; i < n; i++)
      ss << "\t";
    return ss.str();
  } else
    return "";
}

std::string CompoundStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  for (ASTNode *child : this->children) {
    ss << child->prettyPrint(lvl + 1);
  }
  return indent(lvl) + "{\n" + ss.str() + indent(lvl) + "}\n";
}

std::string CompoundStatement::prettyPrintInline(int lvl) {
  std::stringstream ss;
  for (ASTNode *child : this->children) {
    ss << child->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "}\n";
}

std::string ExpressionStatement::prettyPrint(int lvl) {
  return indent(lvl) + children[0]->prettyPrint() + ";\n";
}

std::string ExpressionStatement::prettyPrintInline(int lvl) {
  return "\n" + indent(lvl) + children[0]->prettyPrint() + ";\n";
}

std::string ExpressionStatement::prettyPrintInlineElse(int lvl) {
  return "\n" + indent(lvl) + children[0]->prettyPrint() + ";\n" +
         indent(lvl - 1);
}

std::string CompoundStatement::prettyPrintInlineElse(int lvl) {
  std::stringstream ss;
  for (ASTNode *child : this->children) {
    ss << child->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "} ";
}

std::string IfElseStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  if (this->children[2]) {
    ss << indent(lvl) << "if (" << this->children[0]->prettyPrint() << ")"
       << this->children[1]->prettyPrintInlineElse(lvl + 1);
    ss << "else" << this->children[2]->prettyPrintInlineIf(lvl + 1);
  } else {
    ss << indent(lvl) << "if (" << this->children[0]->prettyPrint() << ")"
       << this->children[1]->prettyPrintInline(lvl + 1);
  }
  return ss.str();
}

std::string IfElseStatement::prettyPrintInline(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string IfElseStatement::prettyPrintInlineIf(int lvl) {
  std::stringstream ss;
  if (this->children[2]) {
    ss << " if (" << this->children[0]->prettyPrint() << ")"
       << this->children[1]->prettyPrintInlineElse(lvl);
    ss << "else" << this->children[2]->prettyPrintInline(lvl);
  } else {
    ss << " if (" << this->children[0]->prettyPrint() << ")"
       << this->children[1]->prettyPrintInline(lvl);
  }
  return ss.str();
}

std::string WhileStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << indent(lvl) << "while (" << this->children[0]->prettyPrint() << ")"
     << this->children[1]->prettyPrintInline(lvl + 1);
  return ss.str();
}

std::string BreakStatement::prettyPrint(int lvl) {
  return indent(lvl) + "break;";
}

std::string ContinueStatement::prettyPrint(int lvl) {
  return indent(lvl) + "continue;";
}

std::string ReturnStatement::prettyPrint(int lvl) {
  if (children[0])
    return indent(lvl) + "return " + children[0]->prettyPrint() + ";\n";
  else
    return indent(lvl) + "return;\n";
}

std::string ReturnStatement::prettyPrintInline(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string ReturnStatement::prettyPrintInlineElse(int lvl) {
  return this->prettyPrintInline(lvl) + indent(lvl - 1);
}

// Methods to generate dot language of graphviz for different AST types.
std::string ASTNode::toGraph() {
  return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
         this->graphWalker() + "}\n}\n";
}

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  for (ASTNode *child : this->children)
    ss << child->prettyPrint(lvl);
  return ss.str();
}

std::string TranslationUnit::graphWalker() {
  if (children.empty() && this->token) {
    std::stringstream ss;
    ss << (unsigned long)this << "[label=<<font point-size='10'>"
       << *this->token << "</font>";
    ss << "> shape=none style=filled fillcolor=lightgrey];\n";
    return ss.str();
  } else {
    std::stringstream ss;
    for (ASTNode *child : children) {
      ss << child->graphWalker();
    }
    return ss.str();
  }
}

std::string Declaration::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=\"" << this->name
     << "\" shape=box style=filled fillcolor=lightsalmon];\n";
  return ss.str();
}

std::string Expression::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ;
  ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << child->graphWalker() << (unsigned long)this << "--"
         << (unsigned long)child << std::endl;
    }
  }
  return ss.str();
}

std::string PrimaryExpression::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->token->getExtra()
     << "> shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

std::string Statement::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << "subgraph cluster_" << (unsigned long)child << "{\n"
         << child->graphWalker() << "}\n";
      ss << (unsigned long)this << "--" << (unsigned long)child << ";\n";
    }
  }
  return ss.str();
}
} // namespace ccc
