#include "../include/common.hpp"

str_t       newLine(str_t &in, str_t toFind = CRLF)
{
	size_t	pos = in.find(toFind);
	str_t	out = in.substr(0, pos);
    if (pos == in.npos)
        in = "";
    else
	    in = in.substr(pos + toFind.size(), in.size() - pos + toFind.size());
	return (out);
}

str_t    str_tolower(str_t const &s)
{
    if (s.size() == 0)
        return (str_t());
    //str_t::const_iterator begin = s.begin(), end = s.end();
    str_t           ret;
    for (size_t i = 0; i < s.size(); i++)
        ret.insert(i, 1, tolower(s[i]));
    return (ret);
}