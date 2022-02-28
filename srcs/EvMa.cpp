#include "EvMa.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"

EvMa::EvMa(config_v &conf)
{
	/* should put all of this in init list -- also put missing stuff (nb_events..)*/

	//_port = strdup(port);	// duplicate because default value
	_cluster_size = conf.size();
	for (config_v::iterator it = conf.begin(); it != conf.end(); it++)
	{
		Server s(*it);
		_cluster[s.id()] = s;
	}
	//_portNb = conf.port()[0];		//warning = should implement multiple listening socket
	//_max_event = conf.client_max();
	_event_nb = 0;

	init_epoll();
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


// void	Server::init_socket()
// {
// 	sockaddr_in servaddr;
// 	servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(_portNb);

// 	if ((_socket_fd = socket(servaddr.sin_family, SOCK_STREAM, 0)) < 0)
// 	{
// 		std::cout << errno;
//         fatal("could not create socket");
// 	}

// 	if ((bind(_socket_fd , (sockaddr*) &servaddr, sizeof(servaddr))) < 0)
//         fatal("could not bind");
    
//     if (listen(_socket_fd , 10) < 0)
// 		fatal("can not listen (i am a cisgender man)");


// 	unlock_socket(_socket_fd);
// }

void	EvMa::init_epoll()
{
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		fatal("epoll creation failed");

	_events = static_cast<event_t*>(calloc(MAXCONN, sizeof(event_t))); //should maybe multiply maxconn by number of serv

	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		it->second.add_to_epoll(_epoll_fd);
	}
	
}

void	EvMa::add_to_interest(int fd, Server *serv)
{
	unlock_socket(fd);
	//enable_keepalive(fd);
	_event.data.fd = fd;
	_event.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_event) == -1)
		fatal("failed to add incoming connection to interest list.");
	//expiry ex = std::make_pair(fd, time_in_ms() + 5000);
	//_clients.push_back(ex);
	Client tmp(fd, serv);
	_clients[fd] = tmp;
	_clients[fd].touch();
	std::cout << "added connection. fd is: " << fd << std::endl;
}

bool    EvMa::is_connected(int fd)
{
	if (!_clients.size())
		return (false);
	for (unsigned long i = 0; i < _clients.size(); i++)
		if (_clients[i].fd() == fd)
			return (true);
	return (false);
}

void	EvMa::incoming_connections(int inc_fd, Server *serv)
{
	int fd;
	struct sockaddr     incoming;		//should probably be before loop start for opti but for now it's fine
    socklen_t			incSize;

	for (;;)
	{
		fd = accept(inc_fd, &incoming, &incSize); //we accept with socket fd as we listen on this on
		if (fd == -1)
		{
			if (errno ==  EAGAIN || errno == EWOULDBLOCK)		//no more requests to accept ! we are done.
				break;
			else
			{
				std::cout << "failed to acccept connection. (should it be fatal ?)"; // idk, should it ??
				break;
			}
		}
		else	//we did accept a connection.
			add_to_interest(fd, serv);
	}
}

void	EvMa::update_expiry(int fd)
{
	_clients[fd].touch();
	for (unsigned long i = 0; i < _expire.size(); i++)
	{
		if (_expire[i]->fd() == fd)
		{
			_expire[i]->touch();
			_expire.push_back(_expire[i]);
			_expire.erase(_expire.begin() + (i - 1));
			return ;
		}
	}
}

Client	&EvMa::find_by_fd(int fd)
{
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].fd() == fd)
			return (_clients[i]);
	}
	//throw exception
	return (_clients[0]);
}

// int	EvMa::read_data(int i)
// {
	
// 	return (0);
// }

int	EvMa::write_data(int i)
{
	Client			&client = _clients[_events[i].data.fd];

	client.respond();
	return (0);
}

int	EvMa::timeout()
{
	if (!_expire.size())				//we do not have any open connections and don't need any timeout
		return (-1);
	int to = (*_expire.begin())->expire() - time_in_ms();
	if (to > 0)
		return (to);
	return (-1);
}

Expire_iterator	EvMa::disconnect_socket(Expire_iterator expired)
{
	if ((*expired)->fd() == 0)
		return (expired);
	std::cout << "closed connection to socket nb " << (*expired)->fd() << std::endl;
	close((*expired)->fd());
	//epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, expired->first, NULL);
	
	_clients.erase((*expired)->fd());
	Expire_List::iterator tmp = expired;
	expired++;
	_expire.erase(tmp);

	
	return (expired);
}

bool	EvMa::is_listen(int fd, Server **serv)
{
	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		if (it->second.is_listen(fd))
		{
			*serv = &(it->second);
			return (true);
		}
	}
	return (false);
}
void	EvMa::loop()
{
	Server *ptr = NULL;
	Server **serv = &ptr;
	for (;;)
	{
		_event_nb = epoll_wait(_epoll_fd, _events, MAXCONN, timeout()); //-1 for timeout means it will block unedfinitely. check if that's the behaviour we want.
		//std::cout << "event_nb = "<<  _event_nb << "\n";
		for (int i = 0; i < _event_nb; i++)
		{
			int fd = _events[i].data.fd;
			uint32_t ev = _events->events;
			//HANDLE ERROR WITH & BITWISE OP (why tho). if error, continue
			if (is_listen(fd, serv))
				incoming_connections(fd, *serv);
			else if (ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				assert(is_connected(fd), "disconnect/ could not find fd");
				for (Expire_iterator ex = _expire.begin(); ex != _expire.end(); ex++)
					if ((*ex)->fd() == fd)
						disconnect_socket(ex);
			}
			else if (find_by_fd(fd).isReady() && ev & EPOLLOUT)
			{
				assert(is_connected(fd), "write/ could not find fd");
				write_data(i);
			}
			else if (ev & EPOLLIN)
			{
				assert(is_connected(fd), "read/ could not find fd");
				Client			&client = find_by_fd(fd);
				update_expiry(i);
				client.add_data();
				//shutdown(fd, 0);
				//	client.respond();
			}
			
		}
		if (!_clients.size())
			continue;
		//_event_nb = 0;
		for (Expire_iterator ex = _expire.begin(); ex != _expire.end() && (*ex)->expire() < time_in_ms(); ex++)
    		ex = disconnect_socket(ex);
		
	}
}