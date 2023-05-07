#include <expression_tree/number.hpp>
#include <expression_tree/exception.hpp>
#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Number::Number(bigint val) 
    : m_value{std::move(val)}
{}

const Number::bigint& Number::Value() const noexcept { return m_value; }

std::unique_ptr<IExpr> Number::Copy() const {
    return Factory::MakeNumber(m_value);
}

std::string Number::ToString() const {
    return m_value.ToString();
}

}