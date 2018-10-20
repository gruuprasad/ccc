#include <iostream>
#include "entry/entry_point_handler.hpp"

int main(int argCount, char **const ppArgs) {
  return EntryPointHandler().handle(argCount, ppArgs);
}

