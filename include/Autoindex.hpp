#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include "Request.hpp"
# include "common.hpp"

class Autoindex
{
    public:
        Autoindex(void);
        Autoindex(Autoindex const &src);
        virtual ~Autoindex(void);

        Autoindex   &operator=(Autoindex const &src);

        static str_t  getPage(const char *route, const char *path, str_t const &host, int port);
    private:
        static str_t  getLink(str_t const &dirEntry, str_t const &dirName, str_t const &host, int port, str_t const &route);
};

#endif
