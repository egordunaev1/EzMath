project(AutoUnitTests)

include(GoogleTest)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test
    GTest::gtest_main
    parsing
    fmt::fmt)
    
add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test
    GTest::gtest_main
    parsing
    fmt::fmt)

add_executable(expressions_test expressions_test.cpp)
target_link_libraries(expressions_test
    GTest::gtest_main
    tree
    parsing
    fmt::fmt)

add_executable(tmp_test test.cpp)
target_link_libraries(tmp_test
    GTest::gtest_main
    Boost::multiprecision
    fmt::fmt)
    
gtest_discover_tests(lexer_test parser_test expressions_test)