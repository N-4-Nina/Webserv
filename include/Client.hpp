#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "flags.hpp"

class Client
{
	public:
		Client(void);
		Client(int fd, Server *serv);
		
		//Client(const Client &ref);
		Client	&operator=(const Client &ref);
		~Client(void);
		void	add_request(); 
		void	add_request(unsigned int error);
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
		Request					_req;
		//std::vector<Request>	_req;
		//str_t					_input;
		raw_str_t				_remain;
		char            		_buff[MAXREAD+1];
		unsigned int			_parse_flags;
		size_t					_read_pos;
		unsigned int			_content_len;
		time_t					_expire;
		bool					_ready;
};

//typedef std::vector<Client>		client_v;
typedef	std::vector<Client *>	Expire_List;
typedef	Expire_List::iterator	Expire_iterator;

#endif