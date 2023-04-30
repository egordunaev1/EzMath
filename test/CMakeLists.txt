project(AutoUnitTests)

include(GoogleTest)
enable_testing()

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test
    GTest::gtest_main
    parsing
    fmt::fmt)
gtest_discover_tests(lexer_test)
