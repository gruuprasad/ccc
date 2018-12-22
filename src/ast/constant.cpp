#include "constant.hpp"
#include <sstream>

namespace ccc {

std::string IntegerConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

std::string CharacterConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

std::string EnumerationConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

} // namespace ccc
