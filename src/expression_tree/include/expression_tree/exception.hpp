#pragma once

#include <string>

namespace ezmath::expression_tree::exception {

class CalcException : public std::exception {
public:
    CalcException(std::string_view msg);
    const char* what() const noexcept override;

private:
    std::string m_msg;
};

}