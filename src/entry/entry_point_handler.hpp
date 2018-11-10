#ifndef C4_ENTRY_POINT_HANDLER_HPP
#define C4_ENTRY_POINT_HANDLER_HPP

#include <fstream>

class EntryPointHandler {
public:
  EntryPointHandler();
  int handle(int argCount, char **ppArgs);
  int tokenize(std::ifstream file, std::string filename);
};

#endif //C4_ENTRY_POINT_HANDLER_HPP
