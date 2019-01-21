SET(ast_SRCS ast_node.cpp
    pretty_printer.cpp
    semantic_analysis.cpp
    )

add_library(ast SHARED ${ast_SRCS})
