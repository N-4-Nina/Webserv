#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Response.hpp"


class Client
{
	public:
		Client(int fd);
		//Client(const Client &ref);
		Client	&operator=(const Client &ref);
		~Client(void);
		void	add_request();
		int		add_data();
		void	respond();
		int		fd();
		char	*buff();
		void	touch();

	private:
		Client(void);
		int						_fd;
		std::vector<Request>	_req;
		str_t					_input;
		char            		_buff[MAXREAD+1];
		unsigned int			_headers_len;
		unsigned int			_content_len;
		time_t					_expire;
};

typedef std::vector<Client>		client_v;

#endif