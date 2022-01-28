#include "EvMa.hpp"

// a terme devra prendre un pointeur sur config
EvMa::EvMa(char	*port = "8000", int max_event = 5)
{
	/* should put all of this in init list -- also put missing stuff (nb_events..)*/

	_port = strdup(port);	// duplicate because default value
	_portNb = atoi(port);
	_max_event = max_event;

	init_socket();			//so maybe private ??
	
}

//a implementer
// EvMa::EvMa(const EvMa &ref)
// {
// }

// EvMa	&EvMa::operator=(const EvMa &ref)
// {
// 	return (*this);
// }

EvMa::~EvMa(void)
{
}

int	EvMa::unlock_socket(int fd)
{
	int flags;
	flags = fcntl(fd,  F_GETFL);
	//if (flags == -1)
		//handle_error
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
		fatal("unlock socket failed");
	return (0);
}

int	EvMa::init_socket()
{
	sockaddr_in servaddr;
	servaddr.sin_family = AF_UNSPEC;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_portNb);

	if ((_socket_fd = socket(servaddr.sin_family, SOCK_STREAM, AI_PASSIVE)) < 0)
        fatal("could not create socket");

	if ((bind(_socket_fd , (sockaddr*) &servaddr, sizeof(servaddr))) < 0)
        fatal("could not bind");
    
    if (listen(_socket_fd , 10) < 0)
        fatal("can not listen (i am a cisgender man)");
}

int	EvMa::init_epoll()
{
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		fatal("epoll creation failed");
	_alloc.allocate(_max_event, _events);

	_event.data.fd = _socket_fd;
	_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl (_epoll_fd, EPOLL_CTL_ADD, _socket_fd, &_event) == -1) /*intialize imterest list*/
		fatal("epoll ctl");
}

int	EvMa::incoming_connections(int i)
{
	for (;;)
	{

	}
}

int	EvMa::read_data(int i)
{

}

void	EvMa::loop()
{
	for (;;)
	{
		_event_nb = epoll_wait(_epoll_fd, _events, _max_event, -1); //-1 for timeout means it will block unedfinitely. check if that's the behaviour we want.
		for (int i = 0; i < _event_nb; i++)
		{
			//HANDLE ERROR WITH & BITWISE OP (why tho). if error, continue
			if (_events[i].data.fd == _socket_fd)
				incoming_connections(i);
			else
				read_data(i);
		}
	}
}