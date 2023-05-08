#include <parsing/parser.hpp>
#include <parsing/exception.hpp>
#include <parsing/token_utils.hpp>

#include <fmt/format.h>

#include <unordered_map>

namespace ezmath::parsing {

Parser::Parser(const std::string_view text)
    : m_lexer{text}
{}  

std::unique_ptr<expression_tree::Expression> Parser::BuildTree() {
    auto tree = ParseExpression();
    if (m_lexer.GetToken()) {
        throw exception::ParserException{"found extra characters at the end of the expression"};
    }
    return tree;
}

std::unique_ptr<expression_tree::Expression> Parser::ParseExpression() {
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

std::unique_ptr<expression_tree::Expression> Parser::ParseSum() {
    std::vector<std::unique_ptr<expression_tree::Expression>> values;
    int sign = ReadSumOperator();

    if (!sign) sign = 1; // First term may not have sign
    do {
        auto next = ParseProduct();
        if (sign == -1) {
            next = Factory::Negate(std::move(next));
        }
        values.emplace_back(std::move(next));
    } while((sign = ReadSumOperator()));

    return Factory::MakeSum(std::move(values));
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

    return token.Type == Token::EType::Symbol 
        || token.Type == Token::EType::Number 
        || token == token::bracket::round::opening
        ? 1 
        : 0;
}

std::unique_ptr<expression_tree::Expression> Parser::ParseProduct() {
    std::vector<std::unique_ptr<expression_tree::Expression>> values;
    while (const auto power = ReadProdOperator()) {
        auto next = ParsePower();
        if (power == -1) {
            next = Factory::MakePower(std::move(next), Factory::MakeNumber(-1));
        }
        values.emplace_back(std::move(next));
    }
    return Factory::MakeProduct(std::move(values));
}


std::unique_ptr<expression_tree::Expression> Parser::ParsePower() {
    auto base = ParseObject();
    if (m_lexer.GetToken() == token::operation::pow) {
        m_lexer.NextToken();
        return Factory::MakePower(std::move(base), ReadArgument());
    }
    return base;
}

std::unique_ptr<expression_tree::Expression> Parser::ParseObject() {
    if (!m_lexer.GetToken()) {
        throw exception::ParserException{"expected object, found end"};
    }

    const auto token = *m_lexer.GetToken();
    
    if (token == token::bracket::round::opening) {
        m_lexer.NextToken();
        
        auto inner = ParseExpression();
        if (m_lexer.GetToken() == token::bracket::round::closing) {
            m_lexer.NextToken();
            return inner;
        }
        throw exception::ParserException{fmt::format("expected ')' to match '('", token.Value)};
    }
    if (token.Type == Token::EType::Number) {
        m_lexer.NextToken();
        return Factory::MakeNumber(token.Value);
    }
    if (token.Type == Token::EType::Symbol) {
        m_lexer.NextToken();
        return Factory::MakeSymbol(token.Value);
    }
    if (token.Type == Token::EType::Command) {
        throw exception::ParserException{"command-object is not implemented yet"};
    }
    throw exception::ParserException{"expected object"};
}

std::unique_ptr<expression_tree::Expression> Parser::ReadArgument() {
    const auto curChar = m_lexer.GetChar();

    if (!curChar) {
        throw exception::ParserException{"cannot read argument, found end"};
    }

    if (std::isdigit(*curChar)) {
        m_lexer.NextChar();
        return Factory::MakeNumber(static_cast<int64_t>(*curChar - '0'));
    }

    if (std::isalpha(*curChar)) {
        m_lexer.NextChar();
        return Factory::MakeSymbol(std::string_view{curChar, 1});
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
    throw exception::ParserException{"expected argument"};
}

std::unique_ptr<expression_tree::Expression> ParseTree(const std::string_view str) {
    return Parser{str}.BuildTree();
}

} // namespace ezmath::parsing