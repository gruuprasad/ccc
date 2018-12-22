#include "expression.hpp"
#include <sstream>

namespace ccc {

std::string Expression::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << this->token->name()
       << this->token->getExtra() << "</font>";
  ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    ss << child->toGraphWalker();
    ss << this->id << " -- " << child->getId()
       << "[taillabel=\"?\" labeldistance=0 labelangle=0 "
          "labelfontcolor=red];\n";
  }
  return ss.str();
}

} // namespace ccc
