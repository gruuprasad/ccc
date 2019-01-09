#ifndef UTILS_HPP
#define UTILS_HPP

#include "ast/ast_node.hpp"
#include <fstream>
#include <sstream>
#include <vector>

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
};
} // namespace ccc
#endif
