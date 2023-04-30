project(parsing)

set(PARSING_SRC
    exception.cpp
    lexer.cpp)

add_library(${PROJECT_NAME} ${PARSING_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE fmt::fmt)