#include "declaration.hpp"
#include <sstream>

namespace ccc {

std::string Declaration::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name
     << "\" shape=box style=filled fillcolor=lightsalmon];\n";
  return ss.str();
}

} // namespace ccc
