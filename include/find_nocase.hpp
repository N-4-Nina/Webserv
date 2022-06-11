#include "common.hpp"


/* find substring while ignoring case */
template<typename T>
size_t find_nocase( const T& str1, const T& str2, size_t offset = 0)
{
    typename T::const_iterator it = std::search( str1.begin() + offset, str1.end(), 
        str2.begin(), str2.end());
    if ( it != str1.end() )
        return it - str1.begin();
    else
        return str1.npos; // not found
}