#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "../include/common.hpp"
# include "Location.hpp"

class Location;

class Config
{
    public:
        Config(void);
        Config(const Config &ref);
        Config &operator=(const Config &ref);
        ~Config(void);
    private:
        str_t               host;
        std::vector<int>    port;
        str_t               server_name;
        strMap              error_page;
        size_t              client_max_body_size;
        str_t               root;
        std::list<str_t>    index;
        str_t               autoindex;
        std::vector<Location>   location;
        strMap              _mimetypes;
};

#endif