#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../include/common.hpp"

enum
{
	R_GET,
	R_POST,
	R_DELETE
};

#define REQ_ISBAD 1

class Request
{
	public:
		Request(int fd = -1);
		//Request(str_t input, int fd, size_t nl_head, size_t nl_body);
		Request(unsigned int error, int fd);
		//Request(const Request &ref);
		Request	&operator=(const Request &ref);
		~Request(void);


		void		reset();
		int			parse_TopLine(str_t &line);
		int			add_Header(str_t line);
		int			add_Body(str_t line);
		bool		isBoundary(str_t line);
		bool		done_Reading();
		bool		over_Read();
		void		set_Error(unsigned int code);
		int	fd();
		unsigned int 		type();
		bool				isBad();
		unsigned	int		&error();
		strMap				&headers();
		std::vector<str_t>	&body();
		str_t					_ressource;		//should maybe be called _route...
		FLAGS					_flags;

	private:
		//int			parse(str_t input);
		int			parse_QueryString(size_t start);
		size_t		parse_Url(str_t const &line);
		
		int						_fd;
		str_t					_boundary;
		size_t					_cl;
		unsigned	int			_type;
		strMap					_headers;
		//URL
		//char					*_body;
		unsigned	int			_read_body;
		unsigned	int			_error;
		size_t					_nl_headers;
		size_t					_nl_body;
		std::vector<str_t>		_body;
		strMap					_queryParam;
};

#endif