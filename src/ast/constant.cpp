#include "constant.hpp"
#include <sstream>

Constant::Constant(int id) : ASTNode(id, "constant") {}

IntegerConstant::IntegerConstant(int id, int constant) : Constant(id) {
  this->constant = constant;
}

std::string IntegerConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

CharacterConstant::CharacterConstant(int id, char constant) : Constant(id) {
  this->constant = constant;
}

std::string CharacterConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

EnumerationConstant::EnumerationConstant(int id, std::string &constant)
    : Constant(id) {
  this->constant = constant;
}

std::string EnumerationConstant::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->constant
     << "\" shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}
