SET(ast_SRCS ast_node.cpp scope_handler.hpp)

add_library(ast SHARED ${ast_SRCS})
