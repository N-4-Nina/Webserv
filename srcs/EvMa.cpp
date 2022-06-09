#include "EvMa.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"

/*
					.--------------.
					| Constructors |
					'--------------'
*/

EvMa::EvMa(config_v &conf)
{
	_cluster_size = conf.size();
	for (config_v::iterator it = conf.begin(); it != conf.end(); it++)
	{
		Server s(*it);
		_cluster[s.id()] = s;
	}
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
	close(_epoll_fd);
	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		it->second.close_fd();
	}
	for (Clients_pool::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		close(it->second.fd());
		it->second.cgi().close_fd();
	}
}

void	EvMa::init_epoll()
{
	_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (_epoll_fd == -1)
		fatal("epoll creation failed");

	_events = static_cast<event_t*>(calloc(MAXCONN, sizeof(event_t))); //should maybe multiply maxconn by number of serv
	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		it->second.add_to_epoll(_epoll_fd);
	}
	
}

void	EvMa::close_all(void)
{
	close(_epoll_fd);
	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		it->second.close_fd();
	}
	for (Clients_pool::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		close(it->first);
		it->second.cgi().close_fd();
	}
}


/*
					.-------------.
					| Evaluations |
					'-------------'
*/

bool    EvMa::is_connected(int fd)
{
	if (!_clients.size())
		return (false);
	return (_clients.count(fd));
}

int	EvMa::is_listen(int fd, Server **serv)
{
	int port;

	for (Cluster::iterator it = _cluster.begin(); it != _cluster.end(); it++)
	{
		if (it->second.is_listen(fd, &port))
		{
			*serv = &(it->second);
			return (port);
		}
	}
	return (0);
}

int	EvMa::timeout()
{
	if (!_expire.size())	//we do not have any open connections and don't need any timeout
		{ return (TIMEOUT); }
	int to = (*_expire.begin())->expire() - time_in_ms();
	if (to > 0)
		return (to);
	return (TIMEOUT);
}


/*
					.--------------------.
					| Handle Connections |
					'--------------------'
*/

void	EvMa::add_to_interest(int fd, Server *serv, int port)
{
	unlock_socket(fd);
	_event.data.fd = fd;
	_event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_event) == -1)
		fatal("failed to add incoming connection to interest list.");
	_clients.insert(std::pair<int, Client>(fd, Client(fd, serv, this, port)));
	_clients[fd].touch();
	_expire.push_back(&_clients[fd]);
	log(serv, &_clients[fd],  "New client connected.");
}

void	EvMa::incoming_connections(int inc_fd, Server *serv, int port)
{
	int fd;
	struct sockaddr     incoming;
    socklen_t			incSize;

	for (;;)
	{
		/* we accept with socket fd as we listen on this on */
		fd = accept(inc_fd, &incoming, &incSize); //
		if (fd == -1)
		{
			if (errno ==  EAGAIN || errno == EWOULDBLOCK)		
				break;			//no more requests to accept ! we are done.
			else
			{
				std::cout << "failed to acccept connection: " << strerror(errno);
				break;
			}
		}
		else	//we did accept a connection.
			add_to_interest(fd, serv, port);
	}
}

void	EvMa::disconnect_socket(int fd, Server *serv, str_t reason)
{
	log(serv, &_clients[fd], "Closed connection: " + reason);
	for (Expire_iterator ex = _expire.begin(); ex != _expire.end(); ex++)
	{
		if ((*ex)  == &_clients[fd])
		{
			_expire.erase(ex);
			break;
		}
	}
	_clients.erase(fd);
	shutdown(fd, SHUT_RDWR);
	close(fd);
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	EvMa::disconnect_socket_ex(Expire_iterator ex)
{
	int fd = (*ex)->fd();
	
	log(_clients[fd]._serv, &_clients[fd], "Closed connection: timeout.");
	_expire.erase(ex);
	_clients.erase(fd);
	shutdown(fd, SHUT_RDWR);
	close(fd);
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}


/*
					.-------------------.
					| Main Program Loop |
					'-------------------'
*/

void	EvMa::loop()
{
	Server *ptr = NULL;
	Server **serv = &ptr;
	for (;;)
	{
		_event_nb = epoll_wait(_epoll_fd, _events, MAXCONN, timeout()); //-1 for timeout means it will block unedfinitely. check if that's the behaviour we want.
		for (int i = 0; i < _event_nb; i++)
		{
			int fd = _events[i].data.fd;
			int port = 0;
			uint32_t ev = _events->events;
			if ((port = is_listen(fd, serv)) != 0)
			{
				incoming_connections(fd, *serv, port);
				continue;
			}
			if (!_clients.count(fd))
			{
				std::cout << "fd " << fd << " does not exist.\n";
			}
			else if (ev & EPOLLERR)
			{
				assert(is_connected(fd), "disconnect/ could not find fd");
				int       error = 0;
				socklen_t errlen = sizeof(error);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
					disconnect_socket(fd, ptr, strerror(error));
				else
					disconnect_socket(fd, ptr, "EPOLLERR");
			}
			else if (_clients[fd].isReady() && ev & EPOLLOUT)
			{
				assert(is_connected(fd), "write/ could not find fd");
				str_t reason;
				if (_clients[fd].respond(reason))
					disconnect_socket(fd, ptr, reason);
			}
			else if (ev & EPOLLIN)
			{
				assert(is_connected(fd), "read/ could not find fd");
				int ret = _clients[fd].add_data();
				//if (!ret)
				//	disconnect_socket(fd, ptr, "Read = 0.");
				if (ret > 0)
					disconnect_socket(fd, ptr, "Read error.");
			}
		}
		/*
			Disconnecting every Clients that timed_out.
		*/
		for (Expire_iterator ex = _expire.begin(); ex != _expire.end() && (*ex)->expire() < time_in_ms(); ex = _expire.begin())
    		{ disconnect_socket_ex(ex); }
	}
}