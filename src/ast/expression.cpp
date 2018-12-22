#include "expression.hpp"
#include <iostream>
#include <sstream>

namespace ccc {

std::string Expression::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ;
  ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << child->toGraphWalker() << this->id << " -- " << child->getId()
         << std::endl;
    }
  }
  return ss.str();
} // namespace ccc

} // namespace ccc
