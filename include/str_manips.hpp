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

/* raw data manips and conversion */

str_t				raw_to_str(raw_str_t raw);
raw_str_t			str_to_raw(str_t str);
raw_str_t           char_to_raw(char *buf, size_t n);
char				*raw_to_char(raw_str_t raw);
raw_str_t           raw_add(raw_str_t one, raw_str_t two);
raw_str_t::iterator raw_find(raw_str_t &str, unsigned const char *tofind, size_t size);
// str_t               raw_newLine(raw_str_t raw);
raw_str_t               raw_newLine(raw_str_t &raw);
