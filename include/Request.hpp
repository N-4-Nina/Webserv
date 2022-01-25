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

		int			parse_QueryString(size_t start);
		size_t		parse_Url(str_t const &line);
		int			parse_TopLine(str_t line);

		unsigned	int			_type;
		strMap					_headers;
		//URL
		str_t					_ressource;		//should maybe be called _route...
		strMap					_queryParam;

};

#endif