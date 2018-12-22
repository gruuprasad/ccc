#include "statement.hpp"

#include <iostream>
#include <sstream>
#include <utility>

namespace ccc {

std::string Statement::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << "subgraph cluster_" << child->getId() << "{\n"
         << child->toGraphWalker() << "}\n";
      ss << this->id << "--" << child->getId() << ";\n";
    }
  }
  return ss.str();
}

} // namespace ccc
