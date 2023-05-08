#pragma once

#include <parsing/lexer.hpp>
#include <expression_tree/expression.hpp>
#include <expression_tree/factory.hpp>

namespace ezmath::parsing {

class Parser {
public:
    Parser(std::string_view text);
    Parser(Parser&&) = default;
    Parser(const Parser&) = delete;

    std::unique_ptr<expression_tree::IExpr> BuildTree();

private:
    using math = expression_tree::math;

    std::unique_ptr<expression_tree::IExpr> ParseExpression();

    int ReadSumOperator();
    std::unique_ptr<expression_tree::IExpr> ParseSum();
    
    int ReadProdOperator();
    std::unique_ptr<expression_tree::IExpr> ParseProduct();
    
    std::unique_ptr<expression_tree::IExpr> ParsePower();
    std::unique_ptr<expression_tree::IExpr> ParseObject();
    std::unique_ptr<expression_tree::IExpr> ReadArgument();

    Lexer m_lexer;
};

std::unique_ptr<expression_tree::IExpr> ParseTree(const std::string_view str);

}