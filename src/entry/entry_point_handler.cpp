#include <iostream>
#include "entry_point_handler.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
