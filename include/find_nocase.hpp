#include "common.hpp"

template<typename T>
bool my_equal(T c1, T c2)
{
    return std::toupper(c1) == std::toupper(c2);
}

/* find substring while ignoring case */
template<typename T>
size_t find_nocase( const T& str1, const T& str2, size_t offset = 0)
{
    typename T::const_iterator it = std::search( str1.begin() + offset, str1.end(), 
        str2.begin(), str2.end(), my_equal<char>);
    if ( it != str1.end() )
        return it - str1.begin();
    else
        return str1.npos; // not found
}