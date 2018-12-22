SET(ast_SRCS constant.cpp ast_node.cpp declaration.cpp expression.cpp statement.cpp statement.cpp
        )

add_library(ast SHARED ${ast_SRCS})
