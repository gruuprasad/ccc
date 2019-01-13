#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include "../utils/macros.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ccc {

// Base class for all nodes in AST. 
class ASTNode {
protected:
  ASTNode() = default;

public:
  virtual ~ASTNode() = default;
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
