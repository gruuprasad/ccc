#include <iostream>
#include "entry_point_handler.hpp"
#include "../lexer/reflex_lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3 && std::string(ppArgs[1]) == "--tokenize") {

    auto filename = ppArgs[2];

    FILE *fd;
    if ((fd = fopen(filename, "r")) == nullptr) {
      std::cerr << "file not found!\r";
      return 1;
    }

    ReflexLexer lexer(fd);
    lexer.lex();

    const auto token_list = lexer.results();
    for (const auto &token : token_list) {
      std::cout << filename << ":" << token << std::endl;
    }
    return 0;
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}


