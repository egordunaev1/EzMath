#include <gtest/gtest.h>
#include <expression_tree/factory.hpp>
#include <ranges>

namespace ezmath::test {

using namespace expression_tree;

class ExpressionsTest : public ::testing::Test {
protected:
    using TTree = std::unique_ptr<Expression>;
    TTree res;

    TTree num(int64_t num) { return Factory::MakeNumber(num); }
    TTree pow(TTree&& a, TTree&& b) { return Factory::MakePower(std::move(a), std::move(b)); }
    TTree var(std::string_view s) { return Factory::MakeSymbol(s); }

    template<class... Args>
    TTree prod(Args... args) { return Factory::MakeProduct(std::forward<Args>(args)...); }
    
    template<class... Args>
    TTree sum(Args... args) { return Factory::MakeSum(std::forward<Args>(args)...); }
};

TEST_F(ExpressionsTest, TestEqProd) {
    const auto TEST1 = prod(num(12), pow(var("x"), var("y")));
    const auto TEST2 = prod(num(12), pow(var("x"), var("y")));
    EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
}

TEST_F(ExpressionsTest, TestEqProdNoOrder) {
    const TTree holders[] = {num(12), pow(var("x"), var("y")), var("\\alpha"), num(29)};
    Expression* values[std::size(holders)];
    for (size_t i = 0; i < std::size(holders); ++i) { values[i] = holders[i].get(); }

    const auto TEST1 = prod(num(12), pow(var("x"), var("y")), var("\\alpha"), num(29));
    do {
        auto TEST2 = prod();
        for (const auto& val : values) {
            TEST2->As<Product>()->Add(val->Copy());
        }
        EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
    } while (std::next_permutation(std::begin(values), std::end(values)));
}

TEST_F(ExpressionsTest, TestEqSum) {
    const auto TEST1 = sum(num(12), pow(var("x"), var("y")));
    const auto TEST2 = sum(num(12), pow(var("x"), var("y")));
    EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
}

TEST_F(ExpressionsTest, TestEqSumNoOrder) {
    const TTree holders[] = {num(12), pow(var("x"), var("y")), var("\\alpha"), num(29), 
                            prod(num(12), pow(var("x"), var("y")), var("\\alpha"), num(29))};
    Expression* values[std::size(holders)];
    for (size_t i = 0; i < std::size(holders); ++i) { values[i] = holders[i].get(); }

    const auto TEST1 = sum(num(12), pow(var("x"), var("y")), var("\\alpha"), num(29), 
                                        prod(num(12), pow(var("x"), var("y")), var("\\alpha"), num(29)));
    do {
        auto TEST2 = sum();
        for (const auto& val : values) {
            TEST2->As<Sum>()->Add(val->Copy());
        }
        EXPECT_TRUE(TEST1->IsEqualTo(*TEST2));
    } while (std::next_permutation(std::begin(values), std::end(values)));
}

}