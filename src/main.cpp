#include <iostream>
#include "entry/entry_point_handler.hpp"

int main(int argCount, char **const ppArgs) {
  std::ios_base::sync_with_stdio(false);
  return EntryPointHandler().handle(argCount, ppArgs);
}

