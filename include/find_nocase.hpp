
#include "common.hpp"

// templated version of my_equal so it could work with both char and wchar_t
template<typename charT>
struct my_equal {
    my_equal( const std::locale& loc ) : loc_(loc) {}
    bool operator()(charT ch1, charT ch2) {
        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
    }
private:
    const std::locale& loc_;
};

// find substring (case insensitive)
template<typename T>
size_t find_nocase( const T& str1, const T& str2, size_t offset = 0, const std::locale& loc = std::locale() )
{
    typename T::const_iterator it = std::search( str1.begin() + offset, str1.end(), 
        str2.begin(), str2.end(), my_equal<typename T::value_type>(loc) );
    if ( it != str1.end() )
        return it - str1.begin();
    else
        return str1.npos; // not found
}