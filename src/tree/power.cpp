#include <tree/factory.hpp>
#include <tree/hash_utils.hpp>
#include <fmt/format.h>

namespace ezmath::tree {

Power::Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp)
    : m_base{std::move(base)}
    , m_exp{std::move(exp)}
{
    if (m_base->Is<Power>()) {
        m_exp = math::multiply(std::move(m_exp), m_base->As<Power>()->DetachExp());
        m_base = m_base->As<Power>()->DetachBase();
    }
}

const IExpr& Power::GetBase() const noexcept { return *m_base; }

const IExpr& Power::Exp() const noexcept { return *m_exp; }

std::unique_ptr<IExpr>&& Power::DetachBase() noexcept { return std::move(m_base); }

std::unique_ptr<IExpr>&& Power::DetachExp() noexcept { return std::move(m_exp); }

std::unique_ptr<IExpr> Power::simplify_ProductBase() {
    if (!(m_base->Is<Product>() && m_exp->Is<Number>() && m_exp->As<Number>()->Value().IsInteger())) {
        return nullptr;
    }

    auto product = m_base->As<Product>();
    std::vector<std::unique_ptr<IExpr>> multipliers;

    auto&& power = math::exp(math::number(std::move(product->DetachCoefficient())), m_exp->Copy());
    multipliers.emplace_back(std::move(power));

    auto&& detachedConstants = product->DetachConstants();
    while (!detachedConstants.empty()) {
        auto&& base = std::move(detachedConstants.extract(detachedConstants.begin()).value().Expression);
        multipliers.emplace_back(math::exp(std::move(base), m_exp->Copy()));
    }

    auto&& detachedVars = product->DetachVariables();
    while (!detachedVars.empty()) {
        auto&& base = std::move(detachedVars.extract(detachedVars.begin()).value().Expression);
        multipliers.emplace_back(math::exp(std::move(base), m_exp->Copy()));
    }

    return math::multiply(std::move(multipliers));
}

std::unique_ptr<IExpr> Power::simplify_SimplifyChildren() {
    math::simplify(m_base);
    math::simplify(m_exp);
    return nullptr;
}

std::unique_ptr<IExpr> extractInteger(std::unique_ptr<IExpr>& base, const BigNum& exp) {
    auto num = boost::multiprecision::numerator(exp.GetImpl());
    auto den = boost::multiprecision::denominator(exp.GetImpl());

    BigNum::Integer r = num % den;
    BigNum::Integer integer = num / den;
    if (r < 0) {
        r += den;
        --integer;
    }
    
    if (integer == 0 || r == 0) {
        return nullptr;
    }

    std::unique_ptr<IExpr> a = math::exp(base->Copy(), math::number(integer.convert_to<BigNum::Rational>()));
    std::unique_ptr<IExpr> b = math::exp(std::move(base), math::number(BigNum::Rational{std::move(r), std::move(den)}));

    math::simplify(a);
    math::simplify(b);

    return math::multiply(std::move(a), std::move(b));
}

BigNum::Integer intPowImpl(const BigNum::Integer& base, const BigNum::Integer& exp) {
    if (exp <= std::numeric_limits<int>::max()) {
        auto n = exp.convert_to<int>();
        if (n == 1) {
            return base;
        }
        return boost::multiprecision::pow(base, n);
    }
    throw exception::CalcException{"unable to exponentiate"};
}

std::optional<BigNum::Integer> intRtImpl(const BigNum::Integer& base, const BigNum::Integer& bigN) {
    if (bigN > std::numeric_limits<int>::max()) {
        throw exception::CalcException{"unable to exponentiate"};
    }
    auto n = bigN.convert_to<int>();
    if (n == 1) {
        return base;
    }

    BigNum::Integer lb = 0;
    BigNum::Integer rb = base;

    while (lb < rb - 1) {
        auto m = (rb - lb) / 2;
        auto cmp = base.compare(boost::multiprecision::pow(m, n));
        switch (cmp) {
            case -1: rb = m; break;
            case 0: rb = m; return BigNum::Integer{m};
            case 1: lb = m; break;
        }
    }
    return std::nullopt;
}

std::unique_ptr<IExpr> intPow(const BigNum& base, const BigNum& exp) {
    auto intBase = base.GetImpl().convert_to<BigNum::Integer>();
    auto numerator = boost::multiprecision::numerator(exp.GetImpl()) * exp.Sign();
    auto denominator = boost::multiprecision::denominator(exp.GetImpl());

    auto step1 = intPowImpl(intBase, numerator);
    auto step2 = intRtImpl(step1, denominator);

    if (step2.has_value()) {
        if (exp.Sign() == -1) {
            return math::number(BigNum::Rational{1, step2.value()});
        }
        return math::number(step2.value().convert_to<BigNum::Rational>());
    }

    return nullptr;
}

std::unique_ptr<IExpr> powNumBase(const BigNum& base, std::unique_ptr<IExpr>& exp) {
    if (base.IsInteger()) {
        return nullptr;
    }

    auto num = math::number(boost::multiprecision::numerator(base.GetImpl()).convert_to<BigNum::Rational>());
    auto den = math::number(boost::multiprecision::denominator(base.GetImpl()).convert_to<BigNum::Rational>());

    std::unique_ptr<IExpr> a = math::exp(std::move(num), exp->Copy());
    std::unique_ptr<IExpr> b = math::inverse(math::exp(std::move(den), std::move(exp)));

    math::simplify(a);
    math::simplify(b);

    return math::multiply(std::move(a), std::move(b));
}

std::unique_ptr<IExpr> Power::simplify_DegenerateCases() {
    if (m_exp->Is<Number>()) {
        auto expNumeric = m_exp->As<Number>();
        if (expNumeric->Value() == 0) {
            return math::number(1);
        }
        if (expNumeric->Value() == 1) {
            return std::move(m_base);
        }

        // a^(4/3)=a*a^(1/3)
        if (auto res = extractInteger(m_base, expNumeric->Value())) {
            return res;
        }
    }

    if (m_base->Is<Number>()) {
        auto baseNumeric = m_base->As<Number>();
        if (baseNumeric->Value() == 1) {
            return math::number(1);
        }

        // if exp is a number (!= 0, checked before), then 0^exp=0
        if (baseNumeric->Value() == 0 && m_exp->Is<Number>()) {
            if (m_exp->Sign() == -1) {
                throw exception::CalcException{"division by zero"};
            }
            return math::number(0);
        }

        if (auto res = powNumBase(baseNumeric->Value(), m_exp)) {
            return res;
        }
    }

    if (m_base->Is<Number>() && m_exp->Is<Number>()) {
        auto baseNumeric = m_base->As<Number>();
        auto expNumeric = m_exp->As<Number>();

        if (auto res = intPow(baseNumeric->Value(), expNumeric->Value())) {
            return res;
        }
    }
    
    return nullptr;
}

std::unique_ptr<IExpr> Power::SimplifyImpl() {
    decltype(&Power::SimplifyImpl) simplifyRules[] = {
        &Power::simplify_SimplifyChildren,
        &Power::simplify_ProductBase,
        &Power::simplify_DegenerateCases
    };

    for (auto rule : simplifyRules) {
        if (auto res = (*this.*rule)()) {
            return res;
        }
    }
    return nullptr;
}

size_t Power::HashImpl() const {
    constexpr size_t RANDOM_BASE = 17690554549982570371u;
    return hash::combine(RANDOM_BASE, m_base->Hash(), m_exp->Hash());
}

bool Power::IsConstant() const {
    return m_base->IsConstant() && m_exp->IsConstant();
}

bool Power::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    return other.Is<Power>() 
        && m_base->IsEqualTo(*other.As<Power>()->m_base)
        && m_exp->IsEqualTo(*other.As<Power>()->m_exp);
}

std::unique_ptr<IExpr> Power::Copy() const {
    return math::exp(m_base->Copy(), m_exp->Copy());
}

std::string Power::ToString() const {
    if (!m_base->Is<Symbol>() && !(m_base->Is<Number>() && m_base->As<Number>()->Value().IsInteger())) {
        return fmt::format("({})^{{{}}}", m_base->ToString(), m_exp->ToString());
    }
    return fmt::format("{}^{{{}}}", m_base->ToString(), m_exp->ToString());
}

}