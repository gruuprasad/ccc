#include <utility>

#ifndef C4_SCOPE_HPP
#define C4_SCOPE_HPP

#include "ast_node.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace ccc {

template <class T> class ScopeHandler {
private:
  std::vector<std::unordered_map<std::string, T *>> scopes;
  unsigned long lvl;

public:
  ScopeHandler() : scopes({std::unordered_map<std::string, T *>()}), lvl(0){};

  void insertDeclaration(const std::string &key, T *val) {
    scopes[lvl][key] = val;
  }

  void printScopes() {
    std::cout << std::endl;
    for (unsigned long i = 0; i <= lvl; i++) {
      for (const auto &kv : scopes[i]) {
        for (unsigned long j = 0; j < i; j++)
          std::cout << "\t";
        std::cout << kv.first << " : " << kv.second->prettyPrint(0)
                  << std::endl;
      }
    }
  }

  void openScope() {
    if (scopes.size() < ++lvl + 1)
      scopes.emplace_back();
    else
      scopes[lvl].clear();
  }

  void closeScope() { lvl--; }
};

} // namespace ccc

#endif // C4_SCOPE_HPP
