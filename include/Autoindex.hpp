#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include "Request.hpp"
# include "common.hpp"

class Autoindex
{
    public:
        Autoindex(void);
        Autoindex(Autoindex const &src);
        ~Autoindex(void);
        Autoindex   &operator=(Autoindex const &src);
        
        static str_t	get_page(const char *route, const char *path);
    private:
        static str_t	get_link(str_t const &dir_entry, str_t const &route);
};

#endif
