#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../include/common.hpp"

class Location
{
	public:
		Location(str_t block);
		~Location();

		void set_autoindex(str_t line);
		void set_index(str_t line);
		void set_fastcgi_pass(str_t line);
		void set_fastcgi_param(str_t line);

		str_t 				get_autoindex() const;
		std::list<str_t>	get_index() const;
		str_t				get_fastcgi_pass() const;
		strMap				get_fastcgi_param() const;
		str_t				get_path() const;

		str_t search_config(str_t config, str_t key);

	private:
		str_t				_autoindex;
		std::list<str_t>	_index;
		str_t				_fastcgi_pass;
		strMap				_fastcgi_param;
		str_t				_path;
		
		Location();
		Location(const Location &rhs);
		Location &operator=(const Location &rhs);
};

std::ostream &operator<<(std::ostream &os, const Location &src);

#endif