project(parsing)

set(PARSING_SRC
    exception.cpp
    lexer.cpp
    parser.cpp)

add_library(${PROJECT_NAME} ${PARSING_SRC})
target_include_directories(${PROJECT_NAME} 
    PUBLIC include
    PUBLIC ${CMAKE_SOURCE_DIR}/src/expression_tree/include)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE fmt::fmt
    PUBLIC expression_tree)