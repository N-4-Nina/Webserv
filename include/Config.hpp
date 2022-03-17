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
		std::vector<int>        ports() const;
		std::vector<str_t>      server_name() const;
		strMap                  error_page() const;
		size_t                  client_max() const;
		str_t                   root() const;
		std::list<str_t>        &index();
		str_t                   autoindex() const;
		location_v   			&location();

	private:
		str_t                   _host;
		std::vector<int>        _ports;
		std::vector<str_t>      _server_name;
		strMap                  _error_page;
		size_t                  _client_max_body_size;
		str_t                   _root;
		std::list<str_t>        _index;
		str_t                   _autoindex;
		location_v   			_location;
	   
		str_t               search_config(str_t config, str_t key);
		std::vector<size_t> search_location(str_t config);

		void set_host_port(str_t line);
		void set_server_name(str_t line);
		void set_error_page(str_t line);
		void set_client_max(str_t line);
		void set_root(str_t line);
		void set_index(str_t line);
		void set_autoindex(str_t line);
		void set_location(str_t line);
		
		Config();
};

std::ostream &operator<<(std::ostream &os, Config &src);
std::vector<Config>	parsing_config(int argc, char **argv);
template <class T> str_t to_string(T n);

#endif