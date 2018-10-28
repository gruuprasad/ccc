#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include <string>
#include <vector>

class ASTNode {
public:
  int id;
  std::vector<ASTNode *> children;
  std::string name;
  virtual std::string toGraphRec();
  ASTNode(int id, std::string name);
  void print();
  std::string toGraph();
  void addChild(ASTNode *child);
};

#endif //C4_ASTNODE_HPP
