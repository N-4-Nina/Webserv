#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../include/common.hpp"
#include "Config.hpp"

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
		Request(int fd = -1, Config *conf = NULL);
		//Request(str_t input, int fd, size_t nl_head, size_t nl_body);
		Request(unsigned int error, int fd);
		//Request(const Request &ref);
		Request	&operator=(const Request &ref);
		~Request(void);


		void		reset();
		int			parse_TopLine(str_t line);
		int			add_Header(str_t line);
		int			add_Body(raw_str_t line);
		bool		isBoundary(raw_str_t line);
		bool		done_Reading();
		bool		over_Read();
		void		set_Error(unsigned int code);
		int	fd();
		unsigned int 		type();
		bool				isBad();
		unsigned	int		&error();
		strMap				&headers();
		std::vector<raw_str_t>	&body();
		str_t					_ressource;		//should maybe be called _route...
		FLAGS					_flags;
		str_t		url_decode(str_t &src);
		str_t		query_string();
		unsigned int	read_body();
		unsigned int	cl();
		Config		*_conf;
	private:
		//int			parse(str_t input);
		int			parse_QueryString(size_t start);
		size_t		parse_Url(str_t const &line);
		
		int						_fd;
		raw_str_t				_boundary;
		size_t					_cl;
		unsigned	int			_type;
		strMap					_headers;
		//URL
		//char					*_body;
		unsigned	int			_read_body;
		unsigned	int			_error;
		

		std::vector<raw_str_t>		_body;
		strMap					_queryParam;
		str_t					_query_string;
};

#endif