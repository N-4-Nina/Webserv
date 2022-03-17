#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../include/common.hpp"
# include <string>

#define LOC_AUTO	1
#define LOC_INDEX	2
#define LOC_ROOT	4
#define LOC_CGI		8
#define	LOC_UPLOAD	16
#define	LOC_METHOD	32

class Location
{
	public:
		Location(str_t block);
		Location(const Location &rhs);
		Location &operator=(const Location &ref);
		~Location();

		str_t 				autoindex() const;
		std::list<str_t>	&index();
		std::list<str_t>	&methods();
		str_t				root() const;
		str_t				cgi_path() const;
		str_t				cgi_extension() const ;
		str_t				route() const ;
		str_t				upload_path() const ;
		FLAGS				flags() const;

		str_t search_config(str_t config, str_t key);

	private:
		std::list<str_t>	_index;
		str_t				_root;
		str_t				_route;
		str_t				_cgi_path;
		str_t				_cgi_extension;
		str_t				_upload_path;
		std::list<str_t>	_methods;

		FLAGS				_flags;
		
		void set_autoindex(str_t line);
		void set_index(str_t line);
		void set_methods(str_t line);
		void set_cgi_path(str_t line);
		void set_cgi_extension(str_t line);
		void set_root(str_t line);
		void set_upload_pass(str_t line);
		void set_upload_path(str_t line);

		// void set_fastcgi_param(str_t line);

		Location();
};

std::ostream &operator<<(std::ostream &os, Location &src);

#endif