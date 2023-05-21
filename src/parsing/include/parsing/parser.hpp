#pragma once

#include <parsing/lexer.hpp>
#include <tree/expression.hpp>
#include <tree/math.hpp>
#include <functional>

namespace ezmath::parsing {

class Parser {
public:
    Parser(std::string_view text);
    Parser(Parser&&) = default;
    Parser(const Parser&) = delete;

    std::unique_ptr<tree::IExpr> BuildTree();

private:
    using math = tree::math;

    std::unique_ptr<tree::IExpr> ParseExpression();

    int ReadSumOperator();
    std::unique_ptr<tree::IExpr> ParseSum();
    
    int ReadProdOperator();
    std::unique_ptr<tree::IExpr> ParseProduct();
    
    std::unique_ptr<tree::IExpr> ParsePower();
    std::unique_ptr<tree::IExpr> ParseObject();
    std::unique_ptr<tree::IExpr> ParseCommand();

    std::optional<Token> ReadOpeningBracket();
    std::optional<Token> ReadClosingBracket();
    std::unique_ptr<tree::IExpr> ReadArgument();

    std::unique_ptr<tree::IExpr> ParseFrac();

    Lexer m_lexer;

    std::unordered_multimap<std::string, std::function<std::unique_ptr<tree::IExpr>(Parser*)>> s_commandParsers {
        {"\\frac", &Parser::ParseFrac}
    };
};

std::unique_ptr<tree::IExpr> ParseTree(const std::string_view str);

}