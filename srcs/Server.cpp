#include "Server.hpp"

Server::Server(){}

Server::Server(Config &conf)
{
	init_listen(conf.ports());
	_portNb = conf.ports().size();
}

Server::Server(const Server &ref)
{
	(void)ref;
}

Server	&Server::operator=(const Server &ref)
{
	if (&ref != this)
	{
		_ls = ref._ls;
		_portNb = ref._portNb;
	}
	return (*this);
}

Server::~Server(void)
{
}

int		Server::id()
{
	return (_id);
}
bool	Server::is_listen(int fd)
{
	for (listen_sockets::iterator it = _ls.begin(); it != _ls.end(); it++)
	{
		if (it->first == fd)
			return (true);
	}
	return (false);
}
void	Server::add_to_epoll(int epoll_fd)
{
	event_t	tmp;
	tmp.events = EPOLLIN | EPOLLET;
	for (listen_sockets::iterator it = _ls.begin(); it != _ls.end(); it++)
	{
		tmp.data.fd = it->first;
		if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, it->first, &tmp) == -1) /*intialize interest list*/
			fatal("epoll add listen");
	}
	
	
}

void	Server::init_listen(int_v ports)
{
	int first = 1;
	for (int_v::iterator it = ports.begin(); it!= ports.end(); it++)
	{
		sockaddr_in servaddr;
		int			socket_fd;
		servaddr.sin_family = AF_INET;
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	servaddr.sin_port = htons(*it);

		if ((socket_fd = socket(servaddr.sin_family, SOCK_STREAM, 0)) < 0)
		{
			std::cout << errno;
       		fatal("could not create socket");
		}

		if ((bind(socket_fd , (sockaddr*) &servaddr, sizeof(servaddr))) < 0)
        	fatal("could not bind");
    
    	if (listen(socket_fd , 10) < 0)
			fatal("can not listen (i am a cisgender man)");

		unlock_socket(socket_fd);
		_ls[socket_fd] = *it;
		if (first && !(first = 0))
			_id = socket_fd;
	}
}