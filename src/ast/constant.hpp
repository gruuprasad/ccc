#ifndef C4_CONSTANT_HPP
#define C4_CONSTANT_HPP

#include "ast_node.hpp"
#include <string>

namespace ccc {

class Constant : public ASTNode {
public:
  explicit Constant(int id) : ASTNode(id, "constant") {}
};

class IntegerConstant : public Constant {
private:
  int constant;
  std::string toGraphWalker() override;

public:
  IntegerConstant(int id, int constant) : Constant(id), constant(constant) {}
};

class CharacterConstant : public Constant {
private:
  char constant;
  std::string toGraphWalker() override;

public:
  CharacterConstant(int id, char constant) : Constant(id), constant(constant) {}
};

class EnumerationConstant : public Constant {
private:
  std::string constant;
  std::string toGraphWalker() override;

public:
  EnumerationConstant(int id, std::string &constant)
      : Constant(id), constant(constant) {}
};

} // namespace ccc

#endif // C4_CONSTANT_HPP
