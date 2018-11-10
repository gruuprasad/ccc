#include <iostream>
#include <fstream>
#include <sstream>
#include "entry_point_handler.hpp"
#include "../lexer/fast_lexer.hpp"

using namespace ccc;

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3) {
    const std::string flagName = std::string(ppArgs[1]);

    char *filename = &ppArgs[2][0];

    for (long i = 1; ppArgs[2][i] != '\0'; i++) {
      if (ppArgs[2][i - 1] == '/') {
        filename = &ppArgs[2][i];
      }
    }

    std::ifstream file = std::ifstream(ppArgs[2]);
    std::vector<Token, std::allocator<Token>> token_list;
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    if (flagName == "--tokenize") {
      auto lexer = FastLexer(buffer, filename, true);
      token_list = lexer.lex();
      if (lexer.fail()) {
        std::cerr << filename << ":" << lexer.getError() << std::endl;
        return 1;
      }
      return 0;
    }
    if (flagName == "--parse") {
      auto lexer = FastLexer(buffer, filename, false);
      token_list = lexer.lex();
      if (lexer.fail()) {
        std::cerr << filename << ":" << lexer.getError() << std::endl;
        return 1;
      }
      return 0;
    }
  }
  return EXIT_FAILURE;
}
