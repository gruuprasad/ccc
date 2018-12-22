#include "statement.hpp"

#include <sstream>
#include <utility>

namespace ccc {

std::string Statement::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name;
  if (this->token) {
    ss << "<br/><font point-size='10'>" << this->token->name();
    if (!this->token->getExtra().empty())
      ss << " " << this->token->getExtra();
    ss << "</font>";
  }
  ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    ss << "subgraph cluster_" << child->getId() << "{\n"
       << child->toGraphWalker() << "}\n";
    ss << this->id << "--" << child->getId() << ";\n";
  }
  return ss.str();
}

} // namespace ccc
