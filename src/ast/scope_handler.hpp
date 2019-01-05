#include <utility>

#ifndef C4_SCOPE_HPP
#define C4_SCOPE_HPP

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

class ScopeHandler {
private:
  std::vector<std::unordered_map<std::string, std::string>> scopes;
  unsigned long lvl;

public:
  ScopeHandler()
      : scopes({std::unordered_map<std::string, std::string>()}), lvl(0){};

  void insertDeclaration(const std::string &key, std::string val) {
    scopes[lvl][key] = std::move(val);
  }

  void printScope() {
    for (unsigned long i = 0; i <= lvl; i++)
      for (const auto &kv : scopes[i]) {
        for (unsigned long j = 0; j < i; j++)
          std::cout << "\t";
        std::cout << kv.first << " : " << kv.second << std::endl;
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

#endif // C4_SCOPE_HPP
