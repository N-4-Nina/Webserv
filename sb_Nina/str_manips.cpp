#include "../include/common.hpp"

str_t    str_toUpper(str_t const &s)
{
    str_t::const_iterator begin = s.begin(), end = s.end();
    str_t           ret;
    for (; begin != end; begin++)
        ret.insert(ret.npos, 1, toupper(*begin));
    return (ret);
}