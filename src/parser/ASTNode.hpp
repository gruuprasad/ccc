#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include <string>
#include <vector>

class ASTNode {
public:
  int id;
  std::string name;
  virtual std::string toGraphRec();
  ASTNode(int id, std::string name);
  std::string toGraph();
};

#endif //C4_ASTNODE_HPP
