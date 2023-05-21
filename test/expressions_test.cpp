#include <gtest/gtest.h>
#include <tree/math.hpp>
#include <parsing/parser.hpp>
#include <ranges>

namespace ezmath::test {

using namespace tree;

class ExpressionsTest : public ::testing::Test {
protected:
    using TTree = std::unique_ptr<IExpr>;
    TTree res;

    TTree num(int64_t num) { return math::number(num); }
    TTree pow(TTree&& a, TTree&& b) { return math::exp(std::move(a), std::move(b)); }
    TTree var(std::string_view s) { return math::symbol(s); }

    TTree prod(const std::vector<TTree>& vals) {
        std::vector<TTree> values;
        for (const auto& val : vals) values.emplace_back(val->Copy());
        return math::multiply(std::move(values)); 
    }
    
    template<class... Args>
    TTree prod(Args... args) { return math::multiply(std::forward<Args>(args)...); }
    
    TTree sum(const std::vector<TTree>& vals) {
        std::vector<TTree> values;
        for (const auto& val : vals) values.emplace_back(val->Copy());
        return math::add(std::move(values)); 
    }
    
    template<class... Args>
    TTree sum(Args... args) { return math::add(std::forward<Args>(args)...); }
};

TEST_F(ExpressionsTest, TestSumFactorOut) {
    auto TEST = "a^{10}+a";
    auto ANSW = "a\\left(1+a^{9}\\right)";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestSumFactorOut2) {
    auto TEST = "2^4a^{10}+2a^3";
    auto ANSW = "2\\left(1+8a^{7}\\right)a^{3}";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestSumNumbersSymbol) {
    auto TEST = "1+2+a+4+5";
    auto ANSW = "12+a";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestPowProdBase) {
    auto TEST = "(ab)^2";
    auto ANSW = "a^{2}b^{2}";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestEqProd) {
    const auto TEST1 = prod(num(12), pow(var("x"), var("y")));
    const auto TEST2 = prod(num(12), pow(var("x"), var("y")));
    EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
}

TEST_F(ExpressionsTest, TestEqProdNoOrder) {
    TTree tmp[] = {num(12), pow(var("x"), var("y")), var("\\alpha"), num(29)};
    std::vector<TTree> values {
        std::make_move_iterator(std::begin(tmp)),
        std::make_move_iterator(std::end(tmp))
    };

    const auto TEST = prod(values);
    do {
        EXPECT_TRUE(prod(values)->IsEqualTo(*TEST));
    } while (std::next_permutation(std::begin(values), std::end(values)));
}

TEST_F(ExpressionsTest, TestEqSum) {
    const auto TEST1 = sum(num(12), pow(var("x"), var("y")));
    const auto TEST2 = sum(num(12), pow(var("x"), var("y")));
    EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
}

TEST_F(ExpressionsTest, TestEqSumNoOrder) {
    TTree tmp[] = {num(12), pow(var("x"), var("y")), var("\\alpha"), num(29), 
                        prod(num(12), pow(var("x"), var("y")), var("\\alpha"), num(29))};
    std::vector<TTree> values {
        std::make_move_iterator(std::begin(tmp)),
        std::make_move_iterator(std::end(tmp))
    };

    const auto TEST = sum(values);
    do {
        EXPECT_TRUE(sum(values)->IsEqualTo(*TEST));
    } while (std::next_permutation(std::begin(values), std::end(values)));
}

TEST_F(ExpressionsTest, TestSumNumbers) {
    auto TEST = "1+2+3+4+5";
    auto ANSW = "15";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestHard) {
    auto TEST = "\\left(\\frac{2a}{2a+b}-\\frac{4a^2}{4a^2+4ab+b^2}\\right)\\div\\left(\\frac{2a}{4a^2-b^2}+\\frac{1}{b-2a}\\right)+\\frac{8a^2}{2a+b}";
    auto ANSW = "15";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

TEST_F(ExpressionsTest, TestHard2) {
    auto TEST = "\\frac{1}{\\frac{1}{x^2}-\\frac{2}{xy}+\\frac{1}{y^2}}";
    auto ANSW = "15";
    EXPECT_NO_THROW(res = parsing::ParseTree(TEST));
    EXPECT_NO_THROW(math::simplify(res));
    EXPECT_EQ(res->ToString(), ANSW);
}

}