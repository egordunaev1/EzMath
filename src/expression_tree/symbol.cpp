#include <expression_tree/symbol.hpp>

namespace ezmath::expression_tree {

Symbol::Symbol(const std::string_view val) : m_value{val} {}

size_t Symbol::TypeID() noexcept {
    return __COUNTER__;
}

size_t Symbol::GetTypeID() const noexcept {
    return TypeID();
}

std::unique_ptr<IExpr> Symbol::Copy() const {
    return std::make_unique<Symbol>(m_value);
}

std::string Symbol::ToString() const {
    return std::string{m_value};
}

}