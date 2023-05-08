#include <gtest/gtest.h>
#include <expression_tree/factory.hpp>
#include <ranges>

namespace ezmath::test {

using namespace expression_tree;

class ExpressionsTest : public ::testing::Test {
protected:
    using TTree = std::unique_ptr<Expression>;
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

}