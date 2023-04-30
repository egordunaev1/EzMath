#pragma once

#include <string>

namespace ezmath::parsing::exception {

class LexerException : public std::exception {
public:
    LexerException(std::string_view msg);
    const char* what() const noexcept override;

private:
    std::string m_msg;
};

}