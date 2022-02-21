#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Response.hpp"


class Client
{
	public:
		Client(void);
		Client(int fd, int serv);
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

	private:
		int						_fd;
		int						_server_id;
		std::vector<Request>	_req;
		str_t					_input;
		char            		_buff[MAXREAD+1];
		size_t					_nl_headers;
		size_t					_nl_body;
		unsigned int			_headers_len;
		unsigned int			_content_len;
		time_t					_expire;
};

//typedef std::vector<Client>		client_v;
typedef	std::vector<Client *>	Expire_List;
typedef	Expire_List::iterator	Expire_iterator;

#endif