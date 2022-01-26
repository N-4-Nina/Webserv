#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../include/common.hpp"

class Location
{
	public:
		Location();
		~Location();
	private:
		str_t				autoindex;
		std::vector<str_t>	index;
		str_t				fastcgi_pass;
		str_t				fastcgi_param;
};

#endif