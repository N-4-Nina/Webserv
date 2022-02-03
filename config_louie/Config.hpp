#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "../include/common.hpp"
# include "Location.hpp"
# include <string>

class Location;

class Config
{
    public:
        Config(str_t config);
        Config(const Config &ref);
        ~Config(void);

        str_t                   host() const;
        std::vector<int>        port() const;
        std::vector<str_t>      server_name() const;
        strMap                  error_page() const;
        size_t                  client_max() const;
        str_t                   root() const;
        std::list<str_t>        index() const;
        str_t                   autoindex() const;
        std::vector<Location>   location() const;
        // strMap                  mimetypes() const;

        str_t               search_config(str_t config, str_t key);
        std::vector<size_t> search_location(str_t config);

    private:
        str_t                   _host;
        std::vector<int>        _port;
        // char                    *_port;
        std::vector<str_t>      _server_name;
        strMap                  _error_page;
        size_t                  _client_max_body_size;
        str_t                   _root;
        std::list<str_t>        _index;
        str_t                   _autoindex;
        std::vector<Location>   _location;

        // not sur if mymetypes are mandatory
        // strMap                  _mimetypes;
       
        void set_host_port(str_t line);
        void set_server_name(str_t line);
        void set_error_page(str_t line);
        void set_client_max(str_t line);
        void set_root(str_t line);
        void set_index(str_t line);
        void set_autoindex(str_t line);
        void set_location(str_t line);

        // not sur if mymetypes are mandatory
        // void set_mimetypes(strMap mimetypes);
        
        Config();
};

std::ostream &operator<<(std::ostream &os, const Config &src);

#endif