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
		Request(str_t input, int fd, size_t nl_head, size_t nl_body);
		//Request(const Request &ref);
		Request	&operator=(const Request &ref);
		~Request(void);
		int	parse(str_t input);
		int	fd();
		unsigned int type();
		str_t					_ressource;		//should maybe be called _route...

		strMap	&headers();
	private:

		int			parse_QueryString(size_t start);
		size_t		parse_Url(str_t const &line);
		int			parse_TopLine(str_t &line);

		int						_fd;
		//size_t					_cl;
		unsigned	int			_type;
		strMap					_headers;
		//URL
		//char					*_body;
		size_t					_nl_headers;
		size_t					_nl_body;
		std::vector<str_t>		_body;
		strMap					_queryParam;

};

#endif