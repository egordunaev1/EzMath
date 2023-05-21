#include <parsing/parser.hpp>
#include <parsing/exception.hpp>
#include <parsing/token_utils.hpp>

#include <fmt/format.h>

#include <unordered_map>

namespace ezmath::parsing {

Parser::Parser(const std::string_view text)
    : m_lexer{text}
{}  

std::unique_ptr<tree::IExpr> Parser::BuildTree() {
    auto tree = ParseExpression();
    if (m_lexer.GetToken()) {
        throw exception::ParserException{"found extra characters at the end of the expression"};
    }
    return tree;
}

std::unique_ptr<tree::IExpr> Parser::ParseExpression() {
    return ParseSum();
}

int Parser::ReadSumOperator() {
    const auto token = m_lexer.GetToken();
    if (token == token::operation::plus) {
        m_lexer.NextToken();
        return 1;
    }
    if (token == token::operation::minus) {
        m_lexer.NextToken();
        return -1;
    }
    return 0;
}

std::unique_ptr<tree::IExpr> Parser::ParseSum() {
    std::vector<std::unique_ptr<tree::IExpr>> values;
    int sign = ReadSumOperator();

    if (!sign) sign = 1; // First term may not have sign
    do {
        auto next = ParseProduct();
        if (sign == -1) {
            next = math::negate(std::move(next));
        }
        values.emplace_back(std::move(next));
    } while((sign = ReadSumOperator()));

    return math::add(std::move(values));
}

int Parser::ReadProdOperator() {
    if (!m_lexer.GetToken()) return 0;
    const auto token = *m_lexer.GetToken();

    if (token == token::command::cdot) {
        m_lexer.NextToken();
        return 1;
    }
    if (token == token::command::div) {
        m_lexer.NextToken();
        return -1;
    }

    return token.Type == Token::EType::Operator
        || token == token::bracket::right
        || token::bracket::IsClosing(token)
        ? 0 : 1;
}

std::unique_ptr<tree::IExpr> Parser::ParseProduct() {
    std::vector<std::unique_ptr<tree::IExpr>> values;
    while (const auto power = ReadProdOperator()) {
        auto next = ParsePower();
        if (power == -1) {
            next = math::inverse(std::move(next));
        }
        values.emplace_back(std::move(next));
    }
    return math::multiply(std::move(values));
}


std::unique_ptr<tree::IExpr> Parser::ParsePower() {
    auto base = ParseObject();
    if (m_lexer.GetToken() == token::operation::pow) {
        m_lexer.NextToken();
        return math::exp(std::move(base), ReadArgument());
    }
    return base;
}


std::optional<Token> Parser::ReadOpeningBracket() {
    bool assert = false;
    if (m_lexer.GetToken() == token::bracket::left) {
        m_lexer.NextToken();
        assert = true;
    }

    auto token = m_lexer.GetToken();
    if (!token) {
        if (assert) {
            throw exception::ParserException{"expected opening bracket after \\left"};
        }
        return std::nullopt;
    }

    if (token::bracket::IsOpening(*token)) {
        m_lexer.NextToken();
        return token;
    }

    if (assert) {
        throw exception::ParserException{"expected opening bracket after \\left"};
    }

    return std::nullopt;
}

std::optional<Token> Parser::ReadClosingBracket() {
    bool assert = false;
    if (m_lexer.GetToken() == token::bracket::right) {
        m_lexer.NextToken();
        assert = true;
    }

    auto token = m_lexer.GetToken();
    if (!token) {
        if (assert) {
            throw exception::ParserException{"expected closing bracket after \\right"};
        }
        return std::nullopt;
    }

    if (token::bracket::IsClosing(*token)) {
        m_lexer.NextToken();
        return token;
    }

    if (assert) {
        throw exception::ParserException{"expected opening bracket after \\right"};
    }

    return std::nullopt;
}

std::unique_ptr<tree::IExpr> Parser::ParseObject() {
    auto openingBracket = ReadOpeningBracket();
    
    if (openingBracket == token::bracket::round::opening) {
        auto inner = ParseExpression();

        if (token::bracket::round::closing != ReadClosingBracket()) {
            throw exception::ParserException{fmt::format("expected ')' to match '('")};
        }

        return inner;
    }
    
    if (openingBracket) {
        throw exception::ParserException{"unexpected opening bracket"};
    }
    if (!m_lexer.GetToken()) {
        throw exception::ParserException{"expected object, found end"};
    }
    auto token = *m_lexer.GetToken();

    if (token.Type == Token::EType::Number) {
        m_lexer.NextToken();
        return math::number(token.Value);
    }
    if (token.Type == Token::EType::Symbol) {
        m_lexer.NextToken();
        return math::symbol(token.Value);
    }
    if (token.Type == Token::EType::Command) {
        return ParseCommand();
    }
    throw exception::ParserException{"expected object"};
}

std::unique_ptr<tree::IExpr> Parser::ParseCommand() {
    if (auto it = s_commandParsers.find(std::string{m_lexer.GetToken()->Value}); it != s_commandParsers.end()) {
        return it->second(this);
    }
    throw exception::ParserException{fmt::format("command {} is not supported currently", m_lexer.GetToken()->Value)};
}

std::unique_ptr<tree::IExpr> Parser::ReadArgument() {
    const auto curChar = m_lexer.GetChar();

    if (!curChar) {
        throw exception::ParserException{"cannot read argument, found end"};
    }

    if (std::isdigit(*curChar)) {
        m_lexer.NextChar();
        return math::number(static_cast<int64_t>(*curChar - '0'));
    }

    if (std::isalpha(*curChar)) {
        m_lexer.NextChar();
        return math::symbol(std::string_view{curChar, 1});
    }

    if (m_lexer.GetToken() == token::bracket::curly::opening) {
        m_lexer.NextToken();
        auto res = ParseExpression();
        if (m_lexer.GetToken() == token::bracket::curly::closing) {
            m_lexer.NextToken();
            return res;
        }
        throw exception::ParserException{"'}' not found"};
    }

    if (m_lexer.GetToken() && m_lexer.GetToken()->Type == Token::EType::Command) {
        return ParseCommand();
    }
    throw exception::ParserException{"expected argument"};
}

std::unique_ptr<tree::IExpr> ParseTree(const std::string_view str) {
    return Parser{str}.BuildTree();
}

std::unique_ptr<tree::IExpr> Parser::ParseFrac() {
    m_lexer.NextToken(); // Skip \\frac
    auto arg1 = ReadArgument();
    auto arg2 = ReadArgument();
    return math::multiply(std::move(arg1), math::inverse(std::move(arg2)));
}

} // namespace ezmath::parsing