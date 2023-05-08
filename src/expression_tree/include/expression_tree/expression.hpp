#pragma once

#include <memory>

namespace ezmath::expression_tree {

class Expression {
public:
    virtual ~Expression() = default;

    constexpr void ToggleSign() { m_sign *= -1; }
    constexpr int Sign() const { return m_sign; };
    constexpr bool IsConstant() const { return m_isConstant; };

    virtual bool IsEqualTo(const Expression& other) const = 0;
    virtual std::unique_ptr<Expression> Copy() const = 0;
    virtual std::string ToString() const = 0;

    template<typename T>
    T* As() noexcept { return static_cast<T*>(this); }

    template<typename T> 
    const T* As() const noexcept { return static_cast<const T*>(this); }

    template<typename T>
    bool Is() const noexcept { return static_cast<bool>(dynamic_cast<const T*>(this)); }

protected:
    bool m_isConstant = true;
    mutable int m_sign = 1;
};

}