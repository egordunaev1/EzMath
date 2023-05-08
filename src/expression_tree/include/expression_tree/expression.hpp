#pragma once

#include <memory>

namespace ezmath::expression_tree {

struct IExpr {
    virtual ~IExpr() = default;

    virtual size_t Hash() const = 0;
    virtual constexpr bool IsConstant() const = 0;
    virtual constexpr int Sign() const = 0;
    virtual bool IsEqualTo(const IExpr& other) const = 0;
    virtual std::unique_ptr<IExpr> Copy() const = 0;
    virtual std::string ToString() const = 0;

    template<typename T>
    T* As() noexcept { return static_cast<T*>(this); }

    template<typename T> 
    const T* As() const noexcept { return static_cast<const T*>(this); }

    template<typename T>
    bool Is() const noexcept { return static_cast<bool>(dynamic_cast<const T*>(this)); }
};

class BaseExpression : public IExpr {
public:
    size_t Hash() const final {
        return m_bufferedHash
             ? m_bufferedHash
             : HashImpl();
    }

protected:
    void OnChange() {
        m_bufferedHash = 0;
    }

private:
    virtual size_t HashImpl() const = 0;

private:
    mutable size_t m_bufferedHash = 0;
};

}