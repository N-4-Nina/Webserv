#pragma once

#include "common.hpp"

str_t	newLine(str_t &in, str_t toFind = CRLF);
str_t	str_toUpper(str_t const &s);

template <class T>
str_t to_string(T n)
{
    std::ostringstream output;
        output << n;
    return (output.str());
}