project(tree)

set(TREE_SRC 
    exception.cpp
    number.cpp
    symbol.cpp
    sum.cpp
    product.cpp
    power.cpp
    bigint.cpp)

add_library(${PROJECT_NAME} ${TREE_SRC})
target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_MP_STANDALONE)
target_include_directories(${PROJECT_NAME}
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC include)
target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::multiprecision
    PUBLIC fmt::fmt)