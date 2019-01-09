SET(ast_SRCS ast_node.cpp
             semantic_analysis.cpp)

add_library(ast SHARED ${ast_SRCS})
