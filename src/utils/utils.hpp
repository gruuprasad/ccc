#ifndef UTILS_HPP
#define UTILS_HPP

#include "ast/ast_node.hpp"
#include <fstream>
#include <sstream>

namespace ccc {

class Utils {
public:
  static std::vector<std::string> split_lines(const std::string &str) {
    std::stringstream ss(str);
    std::string tmp;
    std::vector<std::string> split;
    while (std::getline(ss, tmp, '\n')) {
      while (tmp[tmp.length() - 1] == '\r' || tmp[tmp.length() - 1] == '\n') {
        tmp.pop_back();
      }
      split.push_back(tmp);
    }
    return split;
  }
#if GRAPHVIZ
  static void saveAST(ASTNode *root, const std::string &filename) {
    std::fstream file;
    file.open(filename + ".gv", std::ios::out | std::ios::trunc);
    file.clear();
    file << root->toGraph();
    file.close();

    std::stringstream ss;
    ss << "dot " << filename << ".gv -Tsvg > " << filename << ".svg";
    std::system(ss.str().c_str());
  }
#endif
};
} // namespace ccc
#endif
