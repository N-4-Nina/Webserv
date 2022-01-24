#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../include/common.hpp"

enum
{
	R_GET,
	R_POST,
	R_DELETE
};

class Request
{
	public:
		Request(void);
		Request(str_t input);
		Request(const Request &ref);
		Request	&operator=(const Request &ref);
		~Request(void);
		int	parse(str_t input);
	private:
		//setType();
		int		parse_TopLine(str_t topLine);
		str_t	newLine(str_t &in);
		unsigned	int			_type;
		std::map<str_t, str_t>	_headers;
		str_t					_ressource;		//should maybe be called _route...


};

#endif