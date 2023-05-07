#pragma once

#include <fmt/format.h>

#include <expression_tree/number.hpp>
#include <expression_tree/power.hpp>
#include <expression_tree/product.hpp>
#include <expression_tree/sum.hpp>
#include <expression_tree/symbol.hpp>

#include <concepts>

namespace ezmath::expression_tree {

template<class T>
concept Expr = std::is_convertible_v<T, std::unique_ptr<Expression>>;

struct Factory {
    Factory() = delete;

    static std::unique_ptr<Product> MakeProduct() {
        return std::make_unique<Product>();
    }

    template<Expr... Args>
    static std::unique_ptr<Product> MakeProduct(Args&&... args) {
        auto res = MakeProduct();
        std::unique_ptr<Expression> init[] = {std::move(args)...};
        for (auto& val : init) {
            res->Add(std::move(val));
        }
        return res;
    }

    static std::unique_ptr<Sum> MakeSum() {
        return std::make_unique<Sum>();
    }

    template<Expr... Args>
    static std::unique_ptr<Sum> MakeSum(Args&&... args) {
        auto res = MakeSum();
        std::unique_ptr<Expression> init[] = {std::move(args)...};
        for (auto& val : init) {
            res->Add(std::move(val));
        }
        return res;
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

    template<class T>
    static std::unique_ptr<Number> MakeNumber(const T val) requires std::is_integral_v<T> {
        return std::make_unique<Number>(Number::bigint{val});
    }

    static std::unique_ptr<Number> MakeNumber(Number::bigint val) {
        return std::make_unique<Number>(std::move(val));
    }
};

}