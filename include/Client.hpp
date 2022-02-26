#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define PARSED_CL	1
#define PARSED_CNL	2
#define PARSED_BNL	4

class Client
{
	public:
		Client(void);
		Client(int fd, Server *serv);
		//Client(const Client &ref);
		Client	&operator=(const Client &ref);
		~Client(void);
		void	add_request();
		int		add_data();
		void	respond();
		int		fd();
		time_t	expire();
		char	*buff();
		void	touch();
		bool	isReady();

	private:
		int						_fd;
		int						_server_id;
		Server					*_serv;
		std::vector<Request>	_req;
		str_t					_input;
		char            		_buff[MAXREAD+1];
		size_t					_nl_headers;
		size_t					_nl_body;
		unsigned int			_parse_flags;
		unsigned int			_headers_len;
		unsigned int			_content_len;
		time_t					_expire;
		bool					_ready;
};

//typedef std::vector<Client>		client_v;
typedef	std::vector<Client *>	Expire_List;
typedef	Expire_List::iterator	Expire_iterator;

#endif