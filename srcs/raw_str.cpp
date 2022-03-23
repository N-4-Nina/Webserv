#include "common.hpp"
#include "str_manips.hpp"

raw_str_t   char_to_raw(char *buf, int n)
{
    raw_str_t ret;

    if (n <= 0)
        return (ret);
    for (int i = 0; i < n; i++)
        ret.push_back(buf[i]);
    return (ret);
}

str_t       raw_to_str(raw_str_t raw)
{
    str_t out;

    for (raw_str_t::iterator it = raw.begin(); it != raw.end(); it++)
        out.append(1, *it);
    return (out);
}

raw_str_t       str_to_raw(str_t str)
{
    return(raw_str_t (str.begin(), str.end()));

    //for (str_t::iterator it = str.begin(); it != str.end(); it++)
    //    out.push_back(*it);
    //return (out);
}

char				*raw_to_char(raw_str_t raw)
{
    char *ret = (char*)malloc(raw.size());

    for (size_t i = 0; i < raw.size(); i++)
        ret[i] = raw[i];
    return (ret);
}


raw_str_t::iterator raw_find(raw_str_t &str,  const char *tofind, size_t size, size_t pos)
{
    raw_str_t::iterator it;
    size_t i;
    for (it = str.begin() + pos; it < str.end(); it++)
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


raw_str_t   raw_newLine(raw_str_t &raw, raw_str_t::iterator hint)
{
    raw_str_t::iterator start, pos;
    start = raw.begin();
    if (hint != raw.end())
        pos = hint;
    else
        pos = raw_find(raw, CRLF, 2);
    raw_str_t out(start, pos);
    if (pos == raw.end())
    {
        //raw.clear();
        out.clear();
    }
    else if (pos + 2 <= raw.end())
        raw = raw_str_t(pos+2, raw.end());
    return (out);
}