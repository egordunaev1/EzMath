#pragma once

#include <expression_tree/exception.hpp>
#include <expression_tree/number.hpp>
#include <expression_tree/power.hpp>
#include <expression_tree/product.hpp>
#include <expression_tree/sum.hpp>
#include <expression_tree/symbol.hpp>
#include <expression_tree/negation.hpp>

#include <fmt/format.h>
#include <concepts>

namespace ezmath::expression_tree {

template<class T>
concept Expr = std::is_convertible_v<T, std::unique_ptr<Expression>>;

struct Factory {
    Factory() = delete;

    template<Expr... Args>
    static std::unique_ptr<Product> MakeProduct(Args&&... args) {
        std::unique_ptr<Expression> init[] = {std::move(args)...};
        return std::make_unique<Product>(std::vector<std::unique_ptr<Expression>>{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        });
    }

    static std::unique_ptr<Product> MakeProduct(std::vector<std::unique_ptr<Expression>>&& values) {
        return std::make_unique<Product>(std::move(values));
    }

    template<Expr... Args>
    static std::unique_ptr<Sum> MakeSum(Args&&... args) {
        std::unique_ptr<Expression> init[] = {std::move(args)...};
        return std::make_unique<Sum>(std::vector<std::unique_ptr<Expression>>{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        });
    }

    static std::unique_ptr<Sum> MakeSum(std::vector<std::unique_ptr<Expression>>&& values) {
        return std::make_unique<Sum>(std::move(values));
    }

    static std::unique_ptr<Power> MakePower(std::unique_ptr<Expression>&& base, std::unique_ptr<Expression>&& exp) {
        return std::make_unique<Power>(std::move(base), std::move(exp));
    }

    static std::unique_ptr<Symbol> MakeSymbol(const std::string_view name) {
        return std::make_unique<Symbol>(name);
    }

    static std::unique_ptr<Number> MakeNumber(const std::string_view str) {
        return std::make_unique<Number>(str);
    }

    static std::unique_ptr<Expression> MakeNumber(const int64_t val) {
        return MakeNumber(Number::bigint{val});
    }

    static std::unique_ptr<Expression> MakeNumber(Number::bigint val) {
        return (val.Sign() == -1) ? MakeNegativeNumber(val) : MakePositiveNumber(val);
    }

    template<class T>
    static std::unique_ptr<Number> MakePositiveNumber(T&& val) {
        if (val < 0) {
            throw exception::CalcException{"MakePositiveNumber called with negative argument"};
        }
        return std::make_unique<Number>(Number::bigint{std::forward<T>(val)});
    }

    template<class T>
    static std::unique_ptr<Expression> MakeNegativeNumber(T&& val) {
        if (val > 0) {
            throw exception::CalcException{"MakeNegativeNumber called with positive argument"};
        }
        return Negate(MakePositiveNumber(-Number::bigint{val}));
    }

    static std::unique_ptr<Expression> Negate(std::unique_ptr<Expression>&& val) {
        return std::make_unique<Negation>(std::move(val));
    }
};

}