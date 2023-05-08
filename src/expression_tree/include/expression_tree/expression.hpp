#pragma once

#include <memory>

namespace ezmath::expression_tree {

class Expression {
public:
    virtual ~Expression() = default;

    virtual constexpr bool IsConstant() const = 0;
    virtual constexpr int Sign() const = 0;
    virtual bool IsEqualTo(const Expression& other) const = 0;
    virtual std::unique_ptr<Expression> Copy() const = 0;
    virtual std::string ToString() const = 0;

    template<typename T>
    T* As() noexcept { return static_cast<T*>(this); }

    template<typename T> 
    const T* As() const noexcept { return static_cast<const T*>(this); }

    template<typename T>
    bool Is() const noexcept { return static_cast<bool>(dynamic_cast<const T*>(this)); }
};

}