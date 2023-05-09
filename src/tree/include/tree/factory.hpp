#pragma once

#include <tree/exception.hpp>
#include <tree/number.hpp>
#include <tree/power.hpp>
#include <tree/product.hpp>
#include <tree/sum.hpp>
#include <tree/symbol.hpp>
#include <tree/negation.hpp>

#include <fmt/format.h>
#include <concepts>

namespace ezmath::tree {

template<class T>
concept Expr = std::is_convertible_v<T, std::unique_ptr<IExpr>>;

struct math {
    math() = delete;

    template<Expr... Args>
    static std::unique_ptr<Product> multiply(Args&&... args) {
        std::unique_ptr<IExpr> init[] = {std::move(args)...};
        return std::make_unique<Product>(std::vector<std::unique_ptr<IExpr>>{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        });
    }

    static std::unique_ptr<Product> multiply(std::vector<std::unique_ptr<IExpr>>&& values) {
        return std::make_unique<Product>(std::move(values));
    }

    template<Expr... Args>
    static std::unique_ptr<Sum> add(Args&&... args) {
        std::unique_ptr<IExpr> init[] = {std::move(args)...};
        return std::make_unique<Sum>(std::vector<std::unique_ptr<IExpr>>{
            std::make_move_iterator(std::begin(init)), 
            std::make_move_iterator(std::end(init))
        });
    }

    static std::unique_ptr<Sum> add(std::vector<std::unique_ptr<IExpr>>&& values) {
        return std::make_unique<Sum>(std::move(values));
    }

    static std::unique_ptr<Power> exp(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp) {
        return std::make_unique<Power>(std::move(base), std::move(exp));
    }

    static std::unique_ptr<Symbol> symbol(const std::string_view name) {
        return std::make_unique<Symbol>(name);
    }

    static std::unique_ptr<Number> number(const std::string_view str) {
        return std::make_unique<Number>(str);
    }

    static std::unique_ptr<IExpr> number(const int64_t val) {
        return number(Number::bigint{val});
    }

    static std::unique_ptr<IExpr> number(Number::bigint val) {
        return (val.Sign() == -1) ? negative_number(val) : positive_number(val);
    }

    template<class T>
    static std::unique_ptr<Number> positive_number(T&& val) {
        if (val < 0) {
            throw exception::CalcException{"positive_number called with negative argument"};
        }
        return std::make_unique<Number>(Number::bigint{std::forward<T>(val)});
    }

    template<class T>
    static std::unique_ptr<IExpr> negative_number(T&& val) {
        if (val > 0) {
            throw exception::CalcException{"negative_number called with positive argument"};
        }
        return negate(positive_number(-Number::bigint{val}));
    }

    static std::unique_ptr<IExpr> negate(std::unique_ptr<IExpr>&& val) {
        return std::make_unique<Negation>(std::move(val));
    }
};

}