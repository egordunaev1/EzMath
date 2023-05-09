#include <gtest/gtest.h>
#include <parsing/parser.hpp>
#include <ranges>

namespace ezmath::test {

using namespace parsing;
using namespace tree;

class ParserTest : public ::testing::Test {
protected:
    using TTree = std::unique_ptr<IExpr>;
    TTree res;
};

TEST_F(ParserTest, TestNumber) {
    constexpr auto TEST = "123456";
    constexpr auto ANSW = "123456";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestFloat) {
    constexpr auto TEST = "123456.78";
    constexpr auto ANSW = "\\frac{6172839}{50}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestSymbol) {
    constexpr auto TEST = "a";
    constexpr auto ANSW = "a";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestSum) {
    constexpr auto TEST = "1+a";
    constexpr auto ANSW = "1+a";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestProd) {
    constexpr auto TEST = "2a\\cdot b\\cdot3";
    constexpr auto ANSW = "2 \\cdot 3ab";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestDivision) {
    constexpr auto TEST = "25\\div16+x\\div25\\cdot xy^{27}";
    constexpr auto ANSW = "\\frac{25}{16^{1}}+\\frac{1}{25^{1}}xxy^{27}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestProdOfSums) {
    constexpr auto TEST = "(x+y)(x+25)12(124)y";
    constexpr auto ANSW = "12 \\cdot (124)(x+y)(x+25)y";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestPower) {
    constexpr auto TEST = "5^{1+2}";
    constexpr auto ANSW = "5^{1+2}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestPowerNoBrackets) {
    constexpr auto TEST = "5^ab";
    constexpr auto ANSW = "5^{a}b";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestPowerOfPower) {
    constexpr auto TEST = "(x^y)^z";
    constexpr auto ANSW = "(x^{y})^{z}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestExpression1) {
    constexpr auto TEST = "a^3+3a^2b+3ab^2+b^3";
    constexpr auto ANSW = "a^{3}+3a^{2}b+3ab^{2}+b^{3}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestExpression2) {
    constexpr auto TEST = "2\\cdot2^x";
    constexpr auto ANSW = "2 \\cdot 2^{x}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ParserTest, TestExpression3) {
    constexpr auto TEST = "1\\div12\\cdot x\\div y\\cdot uio";
    constexpr auto ANSW = "\\frac{1}{12^{1}}\\frac{xuio}{y^{1}}";
    ASSERT_NO_THROW(res = ParseTree(TEST));
    EXPECT_EQ(res->ToString(), ANSW);
}

}