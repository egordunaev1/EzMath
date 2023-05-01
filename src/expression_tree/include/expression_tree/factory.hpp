#pragma once

#include <expression_tree/number.hpp>
#include <expression_tree/power.hpp>
#include <expression_tree/product.hpp>
#include <expression_tree/sum.hpp>
#include <expression_tree/symbol.hpp>
#include <ranges>

#include <concepts>

namespace ezmath::expression_tree {

template<class T>
concept Expr = std::same_as<T, std::unique_ptr<IExpr>>;

template<class T>
concept NumberType = std::is_constructible_v<Number, T>;

struct Factory {
    Factory() = delete;

    static std::unique_ptr<Product> MakeProduct(std::vector<std::unique_ptr<IExpr>>&& subExpr, Number&& coef = 1) {
        return std::make_unique<Product>(std::move(subExpr), std::move(coef));
    }

    template<Expr... Args>
    static std::unique_ptr<Product> MakeProduct(Number&& coef, Args&&... args) {
        std::unique_ptr<IExpr> init[] = {std::move(args)...};
        std::vector<std::unique_ptr<IExpr>> subExpr{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        };
        return std::make_unique<Product>(std::move(subExpr), std::move(coef));
    }

    template<Expr... Args>
    static std::unique_ptr<Product> MakeProduct(Args&&... args) {
        std::unique_ptr<IExpr> init[] = {std::move(args)...};
        std::vector<std::unique_ptr<IExpr>> subExpr{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        };
        return std::make_unique<Product>(std::move(subExpr));
    }

    static std::unique_ptr<Sum> MakeSum(std::vector<std::unique_ptr<IExpr>>&& subExpr) {
        return std::make_unique<Sum>(std::move(subExpr));
    }

    template<Expr... Args>
    static std::unique_ptr<Sum> MakeSum(Args&&... args) {
        std::unique_ptr<IExpr> init[] = {std::move(args)...};
        std::vector<std::unique_ptr<IExpr>> subExpr{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        };
        return std::make_unique<Sum>(std::move(subExpr));
    }

    static std::unique_ptr<Power> MakePower(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp) {
        return std::make_unique<Power>(std::move(base), std::move(exp));
    }

    static std::unique_ptr<Symbol> MakeSymbol(const std::string_view name) {
        return std::make_unique<Symbol>(name);
    }

    static std::unique_ptr<Number> MakeNumber(NumberType auto&& val) {
        return std::make_unique<Number>(std::forward<decltype(val)>(val));
    }
};

}