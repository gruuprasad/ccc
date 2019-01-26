#include "entry_point_handler.hpp"
#include "../ast/visitor/graphviz.hpp"
#include "../ast/visitor/pretty_printer.hpp"
#include "../ast/visitor/semantic_analysis.hpp"
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
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    if (flagName == "--tokenize") {
      auto lexer = FastLexer(buffer, path);
      lexer.tokenize();
      if (lexer.fail()) {
        std::cerr << lexer.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (flagName == "--parse") {
      auto parser = FastParser(buffer, path);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      SemanticVisitor sv;
      root->accept(&sv);
      if (sv.fail()) {
        std::cerr << path << ":" << sv.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (flagName == "--print-ast") {
      auto parser = FastParser(buffer, path);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      SemanticVisitor sv;
      root->accept(&sv);
      if (sv.fail()) {
        std::cerr << path << ":" << sv.getError() << std::endl;
        return EXIT_FAILURE;
      }
      PrettyPrinterVisitor pp;
      std::cout << root->accept(&pp);
      return EXIT_SUCCESS;
    } else if (flagName == "--graphviz") {
      auto parser = FastParser(buffer, path);
      auto root = parser.parse();
      if (parser.fail()) {
        std::cerr << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      GraphvizVisitor gv;
      std::cout << root->accept(&gv) << std::endl;
      return EXIT_SUCCESS;
    }
  }
  std::cerr << "?" << std::endl;
  return EXIT_FAILURE;
}
