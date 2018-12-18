#include "entry/entry_point_handler.hpp"
#include <iostream>

int main(int argCount, char **const ppArgs) {
  std::ios_base::sync_with_stdio(false);
  return EntryPointHandler().handle(argCount, ppArgs);
}
