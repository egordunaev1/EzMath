#pragma once

#include <memory>

namespace ezmath::expression_tree {

class IExpr {
public:
    virtual ~IExpr() = default;

    virtual size_t GetTypeID() const noexcept = 0;

    virtual std::unique_ptr<IExpr> Copy() const = 0;
    virtual std::string ToString() const = 0;
};

}