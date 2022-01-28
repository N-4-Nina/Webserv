#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "../include/common.hpp"
# include "Location.hpp"

class Location;

class Config
{
    public:
        Config(str_t config);
        Config(const Config &ref);
        Config &operator=(const Config &ref);
        ~Config(void);

        void set_host_port(str_t line);
        void set_server_name(str_t line);
        void set_error_page(str_t line);
        void set_client_max(str_t line);
        void set_root(str_t line);
        void set_index(str_t line);
        void set_autoindex(str_t line);
        void set_location(str_t line);
        void set_mimetypes(str_t line);

        str_t                   get_host() const;
        char                    *get_port() const;
        str_t                   get_server_name() const;
        strMap                  get_error_page() const;
        size_t                  get_client_max() const;
        str_t                   get_root() const;
        std::list<str_t>        get_index() const;
        str_t                   get_autoindex() const;
        std::vector<Location>   get_location() const;
        strMap                  get_mimetypes() const;

        str_t               search_config(str_t config, str_t key);
        std::vector<size_t> search_location(str_t config);

    private:
        str_t                   _host;
        // std::vector<int>    port;
        char                    *_port;
        str_t                   _server_name;
        strMap                  _error_page;
        size_t                  _client_max_body_size;
        str_t                   _root;
        std::list<str_t>        _index;
        str_t                   _autoindex;
        std::vector<Location>   _location;
        strMap                  _mimetypes;
        
        Config();
};

std::ostream &operator<<(std::ostream &os, const Config &src);

#endif