#ifndef C4_ENTRY_POINT_HANDLER_HPP
#define C4_ENTRY_POINT_HANDLER_HPP
#include <fstream>

namespace ccc {
class EntryPointHandler {
public:
  EntryPointHandler();
  int handle(int argCount, char **ppArgs);
};
} // namespace ccc

#endif // C4_ENTRY_POINT_HANDLER_HPP
