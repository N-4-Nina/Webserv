#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Response.hpp"
#include "Request.hpp"

#include "Server.hpp"
#include "flags.hpp"

class EvMa;
class Client
{
	public:
		Client(void);
		Client(int fd, Server *serv, EvMa *evma);
		
		Client(const Client &ref);
		Client	&operator=(const Client &ref);
		~Client(void);
		void	add_request(); 
		void	add_request(unsigned int error);
		int		add_data();
		int		respond();
		int		fd();
		CGI		&cgi();
		time_t	expire();
		char	*buff();
		void	touch();
		bool	isReady();
		void	reset();
		raw_str_t				_remain;
		Server					*_serv;
		EvMa					*_evma;
	private:
		int						_fd;
		int						_server_id;
		Request					_req;
		Response				_res;
		char            		_buff[MAXREAD+1];
		unsigned int			_parse_flags;
		size_t					_read_pos;
		unsigned int			_content_len;
		time_t					_expire;
		bool					_ready;
};

//typedef std::vector<Client>		client_v;
typedef	std::list<Client *>		Expire_List;
typedef	Expire_List::iterator	Expire_iterator;

#endif