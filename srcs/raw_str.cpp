#include "common.hpp"
#include "str_manips.hpp"

raw_str_t   char_to_raw(char *buf, size_t n)
{
    raw_str_t ret;

    for (size_t i = 0; i < n; i++)
        ret.push_back(static_cast<unsigned char>(buf[i]));
    return (ret);
}

str_t       raw_to_str(raw_str_t raw)
{
    str_t out;

    for (raw_str_t::iterator it = raw.begin(); it != raw.end(); it++)
        out.append(1, static_cast<char>(*it));
    std::cout << out;
    return (out);
}

raw_str_t       str_to_raw(str_t str)
{
    raw_str_t out;

    for (str_t::iterator it = str.begin(); it != str.end(); it++)
        out.push_back(static_cast<unsigned char>(*it));
    return (out);
}

char				*raw_to_char(raw_str_t raw)
{
    char *ret = (char*)malloc(raw.size());

    for (size_t i = 0; i < raw.size(); i++)
        ret[i] = raw[i];
    return (ret);
}


raw_str_t::iterator raw_find(raw_str_t &str, unsigned const char *tofind, size_t size)
{
    raw_str_t::iterator it;
    size_t i;
    for (it = str.begin(); it != str.end(); it++)
    {
        for (i = 0; i < size && *(it + i) == tofind[i]; i++);
        if (i == size)
            return (it);
    }
    return (str.end());
}

raw_str_t   raw_add(raw_str_t one, raw_str_t two)
{
    raw_str_t ret;
    for (raw_str_t::iterator it = one.begin(); it != one.end(); it++)
        ret.push_back(*it);
    for (raw_str_t::iterator it = two.begin(); it != two.end(); it++)
        ret.push_back(*it);
    return (ret);
}

// str_t   raw_newLine(raw_str_t raw)
// {
//     str_t str = raw_to_str(raw);
//     return (newLine(str));
// }

raw_str_t   raw_newLine(raw_str_t &raw)
{
    raw_str_t::iterator start, end;
    start = raw.begin();
    end = raw_find(raw, UCRLF, 2);
    size_t pos = raw_find(raw, UCRLF, 2) - raw.begin();
    raw_str_t out(start, end);
    if (end == raw.end())
        raw.clear();
    else
        raw = raw_str_t(pos+2, raw.size() - pos + 2);
    return (out);
}