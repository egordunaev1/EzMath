#pragma once

#include <memory>

namespace ezmath::expression_tree {

class IExpr {
public:
    virtual ~IExpr() = default;

    virtual std::unique_ptr<IExpr> Copy() const = 0;
    virtual std::string ToString() const = 0;

    template<typename T> 
    T* As() noexcept { return static_cast<T*>(this); }

    template<typename T>
    const T* As() const noexcept { static_cast<T*>(this); }

    template<typename T>
    bool Is() const noexcept { return static_cast<bool>(dynamic_cast<T*>(this)); }
};

}