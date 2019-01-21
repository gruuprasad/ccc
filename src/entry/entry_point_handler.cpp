#include "entry_point_handler.hpp"
#include "../ast/graphviz.hpp"
#include "../ast/semantic_analysis.hpp"
#include "../lexer/fast_lexer.hpp"
#include "../parser/fast_parser.hpp"
#include "../utils/utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace ccc;

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3) {
    const std::string flagName = std::string(ppArgs[1]);
    std::string path = ppArgs[2];
    std::ifstream file = std::ifstream(path);
    auto filename = path.substr(path.find_last_of("/\\") + 1);
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    if (flagName == "--tokenize") {
      auto lexer = FastLexer(buffer, filename);
      lexer.tokenize();
      if (lexer.fail()) {
        std::cerr << lexer.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (flagName == "--parse") {
      auto parser = FastParser(buffer, filename);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      auto gv = SemanticVisitor();
      root->accept(&gv);
      if (gv.fail()) {
        std::cerr << filename << ":" << gv.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (flagName == "--print-ast") {
      auto parser = FastParser(buffer, filename);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      auto gv = SemanticVisitor();
      root->accept(&gv);
      if (gv.fail()) {
        std::cerr << filename << ":" << gv.getError() << std::endl;
        return EXIT_FAILURE;
      }
      std::cout << root->prettyPrint(0);
      return EXIT_SUCCESS;
    } else if (flagName == "--graphviz") {
      auto parser = FastParser(buffer, filename);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      auto gv = GraphvizVisitor();
      std::cout << root->graphviz(&gv) << std::endl;
      return EXIT_SUCCESS;
    }
  }
  std::cerr << "?" << std::endl;
  return EXIT_FAILURE;
}
