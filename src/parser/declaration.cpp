#include <sstream>
#include "declaration.hpp"

Declaration::Declaration(int id) : ASTNode(id, "declaration") {
}

std::string Declaration::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=house style=filled fillcolor=lightsalmon];\n";
  return ss.str();
}