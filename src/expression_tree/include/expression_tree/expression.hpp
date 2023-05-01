#pragma once

#include <memory>

namespace ezmath::expression_tree {

class IExpr {
public:
    virtual ~IExpr() = default;

    virtual std::unique_ptr<IExpr> Copy() const = 0;
    virtual std::string ToString() const = 0;

    template<typename T> 
    T* As() { return (T*)(this); }

    template<typename T>
    bool Is() { return static_cast<bool>(dynamic_cast<T*>(this)); }
};

}