#include <expression_tree/power.hpp>
#include <fmt/format.h>

namespace ezmath::expression_tree {

Power::Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp)
    : m_base{std::move(base)}
    , m_exp{std::move(exp)}
{}

size_t Power::TypeID() noexcept {
    return __COUNTER__;
}

size_t Power::GetTypeID() const noexcept {
    return TypeID();
}

std::unique_ptr<IExpr> Power::Copy() const {
    return std::make_unique<Power>(m_base->Copy(), m_exp->Copy());
}

std::string Power::ToString() const {
    return fmt::format("{}^{}", m_base->ToString(), m_exp->ToString());
}

}