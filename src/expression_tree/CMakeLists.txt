project(expression_tree)

set(TREE_SRC 
    exception.cpp
    number.cpp
    symbol.cpp
    sum.cpp
    product.cpp
    power.cpp)

add_library(${PROJECT_NAME} ${TREE_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
    PRIVATE fmt::fmt)