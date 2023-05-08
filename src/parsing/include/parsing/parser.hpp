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

    std::unique_ptr<expression_tree::Expression> BuildTree();

private:
    using Factory = expression_tree::Factory;

    std::unique_ptr<expression_tree::Expression> ParseExpression();

    int ReadSumOperator();
    std::unique_ptr<expression_tree::Expression> ParseSum();
    
    int ReadProdOperator();
    std::unique_ptr<expression_tree::Expression> ParseProduct();
    
    std::unique_ptr<expression_tree::Expression> ParsePower();
    std::unique_ptr<expression_tree::Expression> ParseObject();
    std::unique_ptr<expression_tree::Expression> ReadArgument();

    Lexer m_lexer;
};

std::unique_ptr<expression_tree::Expression> ParseTree(const std::string_view str);

}