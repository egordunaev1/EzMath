#include <expression_tree/power.hpp>
#include <expression_tree/factory.hpp>
#include <fmt/format.h>

namespace ezmath::expression_tree {

Power::Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp)
    : m_base{std::move(base)}
    , m_exp{std::move(exp)}
{
    if (m_base->Is<Power>()) {
        auto base = m_base->As<Power>();
        m_base = std::move(base->m_base);
        m_exp = Factory::MakeProduct(std::move(base->m_exp), std::move(m_exp));
    }
}

std::unique_ptr<IExpr> Power::Copy() const {
    return Factory::MakePower(m_base->Copy(), m_exp->Copy());
}

std::string Power::ToString() const {
    return fmt::format("{}^{}", m_base->ToString(), m_exp->ToString());
}

}