#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include <string>
#include <vector>

class ASTNode {
protected:
  int id;
  std::string name;
public:
  int getId() const;
  ASTNode(int id, std::string name);
  virtual std::string toGraphRec();
  std::string toGraph();
};

#endif //C4_ASTNODE_HPP
