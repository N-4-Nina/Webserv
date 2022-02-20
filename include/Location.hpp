#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../include/common.hpp"
# include <string>

class Location
{
	public:
		Location(str_t block);
		Location(const Location &rhs);
		~Location();

		str_t 				autoindex() const;
		std::list<str_t>	index() const;
		str_t				root() const;
		str_t				cgi_path() const;
		str_t				cgi_extension() const ;
		// strMap				fastcgi_param() const;

		str_t search_config(str_t config, str_t key);

	private:
		str_t				_autoindex;
		std::list<str_t>	_index;
		str_t				_root;
		str_t				_cgi_path;
		str_t				_cgi_extension;

		// strMap				_fastcgi_param;
		
		void set_autoindex(str_t line);
		void set_index(str_t line);
		void set_cgi_path(str_t line);
		void set_cgi_extension(str_t line);

		// void set_fastcgi_param(str_t line);

		Location();
};

std::ostream &operator<<(std::ostream &os, const Location &src);

#endif