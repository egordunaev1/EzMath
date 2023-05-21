#include <tree/math.hpp>
#include <tree/hash_utils.hpp>
#include <unordered_set>
#include <ranges>

namespace ezmath::tree {

std::tuple<Number::bigint, std::unique_ptr<Product>, std::unique_ptr<Product>> DecomposeTerm(Term& term) {
    if (!term.Expression->Is<Product>()) {
        if (term.Expression->IsConstant()) {
            return {1, math::multiply(std::move(term.Expression)), math::multiply()};
        }
        return {1, math::multiply(), math::multiply(std::move(term.Expression))};
    }
    
    auto product = term.Expression->As<Product>();
    
    auto coefficient = product->DetachCoefficient();
    
    auto constants = product->DetachConstants();
    std::vector<std::unique_ptr<IExpr>> constMultipliers;
    constMultipliers.reserve(constants.size());
    while (!constants.empty()) {
        auto res = std::move(constants.extract(constants.begin()).value().Expression);
        constMultipliers.emplace_back(std::move(res));
    }

    auto variables = product->DetachVariables();
    std::vector<std::unique_ptr<IExpr>> variableMultipliers;
    variableMultipliers.reserve(variables.size());
    while (!variables.empty()) {
        auto res = std::move(variables.extract(variables.begin()).value().Expression);
        variableMultipliers.emplace_back(std::move(res));
    }

    return {std::move(coefficient), math::multiply(std::move(constMultipliers)), math::multiply(std::move(variableMultipliers))};
}

Term::Term(std::unique_ptr<IExpr>&& expr) 
    : Expression{std::move(expr)}
{
    if (Expression->Is<Product>()) {
        constexpr size_t RANDOM_BASE = 3906861806704847745u;
        Hash = hash::asymmetric_hash(RANDOM_BASE, Expression->As<Product>()->GetVariables());
    }
    Hash = Expression->Hash();
}

bool operator==(const Term& lhs, const Term& rhs) { 
    if (lhs.Hash != rhs.Hash) {
        return false;
    }

    if (lhs.Expression->Is<Product>() != rhs.Expression->Is<Product>()) {
        return false;
    }

    if (lhs.Expression->Is<Product>()) {
        return lhs.Expression->As<Product>()->GetVariables() == rhs.Expression->As<Product>()->GetVariables();
    }

    return lhs.Expression->IsEqualTo(*rhs.Expression);
}

size_t TermHash::operator()(const Term& term) const {
    return term.Hash;
}

Sum::Sum(std::vector<std::unique_ptr<IExpr>>&& values) 
    : m_constant{0}
{
    for (auto& val : values) {
        Add(std::move(val));
    }
}

void Sum::Add(std::unique_ptr<IExpr>&& subExpr) {
    if (!subExpr) {
        return;
    }

    if (subExpr->Is<Number>()) {
        m_constant += subExpr->As<Number>()->Value();
        return;
    }

    if (subExpr->Is<Sum>()) {
        auto subSum = subExpr->As<Sum>();
        m_constant += subSum->m_constant;
        m_terms.merge(subSum->m_terms);
        return;
    }

    m_terms.emplace(std::move(subExpr));
}

const Sum::ConstantType& Sum::GetConstant() const noexcept {
    return m_constant;
}

const Sum::ValueType& Sum::GetTerms() const noexcept {
    return m_terms;
}

Sum::ConstantType&& Sum::DetachConstant() noexcept {
    return std::move(m_constant);
}

Sum::ValueType&& Sum::DetachTerms() noexcept {
    return std::move(m_terms);
}

bool Sum::IsConstant() const {
    return std::all_of(m_terms.begin(), m_terms.end(), 
        [](const auto& val){ return val.Expression->IsConstant(); });
}

int Sum::Sign() const {
    const static auto extractHash = [](const std::unique_ptr<IExpr>& exp){
        if (!exp->Is<Product>()) {
            return exp->Hash();
        }
        auto term = Term{exp->As<Product>()->Copy()};
        auto [_, l, r] = DecomposeTerm(term);
        std::unique_ptr<IExpr> tmp = math::multiply(std::move(l), std::move(r));
        math::simplify(tmp);
        return tmp->Hash();
    };

    auto result = std::max_element(m_terms.begin(), m_terms.end(), [](const auto& a, const auto& b) {
        const size_t aHash = extractHash(a.Expression);
        const size_t bHash = extractHash(b.Expression);
        return aHash < bHash;
    });

    return result->Expression->Sign();
}

bool Sum::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }

    if (!other.Is<Sum>()) {
        return false;
    }

    const auto otherSum = other.As<Sum>();
    return (m_terms == otherSum->m_terms) && (m_constant == otherSum->m_constant);
}

auto TermToMap(Term& term) {
    std::unordered_map<Multiplier, Number::bigint, MultiplierHash> res;
    auto [coef, constPart, varPart] = DecomposeTerm(term);

    auto constPartTmp = constPart->As<Product>()->DetachConstants();
    while (!constPartTmp.empty()) {
        auto val = std::move(constPartTmp.extract(constPartTmp.begin()).value());
        if (GetExp(val).Is<Number>()) {
            auto [base, exp] = DecomposePower(val);
            res.emplace(std::move(base), exp->As<Number>()->DetachValue());
            continue;
        }
        res.emplace(std::move(val.Expression), 1);
    }

    auto varPartTmp = varPart->As<Product>()->DetachVariables();
    while (!varPartTmp.empty()) {
        auto val = std::move(varPartTmp.extract(varPartTmp.begin()).value());
        if (GetExp(val).Is<Number>()) {
            auto [base, exp] = DecomposePower(val);
            res.emplace(std::move(base), exp->As<Number>()->DetachValue());
            continue;
        }
        res.emplace(std::move(val.Expression), 1);
    }

    if (coef != 1 && coef != -1) {
        res.emplace(math::number(coef), 1);
    }

    return res;
}

std::unique_ptr<IExpr> applyFactorOutTerms(std::unique_ptr<IExpr>&& exprOut, Sum::ValueType& terms) {
    if (exprOut->Is<Number>() && (exprOut->As<Number>()->Value() == 1)) {
        return nullptr;
    }

    auto exprIn = math::inverse(exprOut->Copy());
    math::simplify(exprIn);

    std::vector<std::unique_ptr<IExpr>> newTerms;
    newTerms.reserve(terms.size());
    while (!terms.empty()) {
        auto val = std::move(terms.extract(terms.begin()).value().Expression);
        std::unique_ptr<IExpr> term = math::multiply(exprIn->Copy(), std::move(val));
        math::simplify(term);
        newTerms.emplace_back(std::move(term));
    }
    std::unique_ptr<IExpr> res = math::multiply(math::add(std::move(newTerms)), std::move(exprOut));
    math::simplify(res);
    return res;
}

BigNum gcd(const std::vector<BigNum>& values) {
    auto [resnum, resden] = values.front().Decompose();

    for (auto it = ++values.begin(); it != values.end(); ++it) {
        auto [num, den] = it->Decompose();
        resnum = boost::multiprecision::gcd(resnum, num);
        resden = boost::multiprecision::lcm(resden, den);
    }
    return BigNum::Rational{std::move(resnum), std::move(resden)};
}

std::unique_ptr<IExpr> Sum::simplify_FactorOutCoeffs() {
    std::vector<Number::bigint> coeffs;
    coeffs.reserve(m_terms.size());
    for (const auto& term : m_terms) {
        if (term.Expression->Is<Product>()) {
            coeffs.emplace_back(term.Expression->As<Product>()->GetCoefficient());
            continue;
        }
        coeffs.emplace_back(1);
    }
    if (m_constant != 0) {
        coeffs.emplace_back(m_constant);
    }
    auto gcdNumeric = gcd(coeffs) * Sign();

    if (gcdNumeric == 1) {
        return nullptr;
    }

    auto gcdInv = math::inverse(math::number(gcdNumeric));
    math::simplify(gcdInv);

    Sum newSum{{}};

    std::unique_ptr<IExpr> val = math::multiply(gcdInv->Copy(), math::number(std::move(m_constant)));
    math::simplify(val);
    newSum.Add(std::move(val));

    while (!m_terms.empty()) {
        val = std::move(m_terms.extract(m_terms.begin()).value().Expression);
        val = math::multiply(gcdInv->Copy(), std::move(val));
        math::simplify(val);
        newSum.Add(std::move(val));
    }

    *this = std::move(newSum);
    return math::number(gcdNumeric);
}

std::unique_ptr<IExpr> Sum::simplify_FactorOutTerms() {
    auto gcdNumeric = simplify_FactorOutCoeffs();

    std::vector<Term> terms;
    for (const auto& term : m_terms) {
        terms.emplace_back(term.Expression->Copy());
    }

    if (m_constant != 0) {
        terms.emplace_back(math::number(m_constant));
    }

    auto res = TermToMap(terms.back());
    terms.pop_back();

    for (auto& term : terms) {
        auto map = TermToMap(term);

        for (auto it = res.begin(); it != res.end();) {
            auto fndIt = map.find(it->first);

            if (fndIt == map.end()) {
                it = res.erase(it);
                continue;
            }
            
            it->second = std::min(it->second, fndIt->second);
    
            if (it->second == 0) {
                it = res.erase(it);
                continue;
            }

            ++it;
        }
    }

    if (res.empty()) {
        if (gcdNumeric) {
            auto newSum = math::add();
            *newSum = std::move(*this);
            return math::multiply(std::move(gcdNumeric), std::move(newSum));
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<IExpr>> multipliersOut;
    multipliersOut.reserve(res.size());

    while (!res.empty()) {
        auto it = res.extract(res.begin());
        auto [base, power] = std::make_pair(std::move(it.key()), std::move(it.mapped()));
        multipliersOut.emplace_back(math::exp(std::move(base.Expression), math::number(std::move(power))));
    }

    auto applied = applyFactorOutTerms(math::multiply(std::move(multipliersOut)), m_terms);

    if (gcdNumeric) {
        if (applied) {
            return math::multiply(std::move(applied), std::move(gcdNumeric));
        }
        auto newSum = math::add();
        *newSum = std::move(*this);
        applied = math::multiply(std::move(newSum), std::move(gcdNumeric));
    }

    math::simplify(applied);

    return applied;
}

std::unique_ptr<IExpr> Sum::simplify_AddLikeTerms() {
    if (m_terms.size() <= 1) {
        return nullptr;
    }

    Sum res{{}};

    while (!m_terms.empty()) {
        auto [begin, end] = m_terms.equal_range(*m_terms.begin());

        if (std::next(begin) == end) {
            res.m_terms.emplace(std::move(m_terms.extract(begin).value().Expression));
            continue;
        }

        std::vector<std::unique_ptr<IExpr>> coefs;
        auto [coef1, constPart1, varPart] = DecomposeTerm(m_terms.extract(begin++).value());
        coefs.emplace_back(math::multiply(math::number(std::move(coef1)), std::move(constPart1)));

        for (auto it = begin; it != end;) {
            auto [coef1, constPart1, _] = DecomposeTerm(m_terms.extract(it++).value());
            coefs.emplace_back(math::multiply(math::number(std::move(coef1)), std::move(constPart1)));
        }

        auto newCoef = math::add(std::move(coefs));
        res.m_terms.emplace(math::multiply(std::move(newCoef), std::move(varPart)));
    }

    *this = std::move(res);
    return nullptr;
}

std::unique_ptr<IExpr> Sum::simplify_SimplifyChildren() {
    Sum res{{}};
    res.m_constant = std::move(m_constant);
    res.m_terms.reserve(m_terms.size());

    while (!m_terms.empty()) {
        auto val = std::move(m_terms.extract(m_terms.begin()).value().Expression);

        math::simplify(val);
        res.Add(std::move(val));
    }

    *this = std::move(res);
    return nullptr;
}

std::unique_ptr<IExpr> Sum::simplify_DegenerateCases() {
    if (m_terms.empty()) {
        return math::number(std::move(m_constant));
    }
    if ((m_terms.size() == 1) && (m_constant == 0)) {
        return std::move(m_terms.extract(m_terms.begin()).value().Expression);
    }
    return nullptr;
}

std::unique_ptr<IExpr> Sum::SimplifyImpl() {
    decltype(&Sum::SimplifyImpl) simplifyRules[] = {
        &Sum::simplify_SimplifyChildren,
        &Sum::simplify_DegenerateCases,
        &Sum::simplify_AddLikeTerms,
        &Sum::simplify_DegenerateCases,
        &Sum::simplify_FactorOutTerms,
        &Sum::simplify_DegenerateCases
    };

    for (auto rule : simplifyRules) {
        if (auto res = (*this.*rule)()) {
            return res;
        }
    }
    return nullptr;
}

size_t Sum::HashImpl() const {
    constexpr size_t RANDOM_BASE = 18251384670654659732u;   
    return hash::asymmetric_hash(RANDOM_BASE, m_terms);
}

std::unique_ptr<IExpr> Sum::Copy() const {
    constexpr auto copy = [](const auto& val) { return val.Expression->Copy(); };

    auto newSum = math::add();
    newSum->m_constant = m_constant;
    std::transform(m_terms.begin(), m_terms.end(), std::inserter(newSum->m_terms, newSum->m_terms.end()), copy);
    return newSum;
}

std::string Sum::ToString() const {
    std::string res;
    res.reserve(256);

    if (m_terms.empty()) {
        return m_constant.ToString();
    }

    if (m_constant != 0) {
        res.append(m_constant.ToString());
    }

    for (const auto& term : m_terms) {
        auto termStr = term.Expression->ToString();
        if (!termStr.starts_with('-') && !res.empty()) {
            res.push_back('+');
        }
        res.append(termStr);
    }
    return res;
}

}