#include "EvMa.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

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

/*	
	EvMa doesn't have all the requirements for a canonical class
	because I don't want it to. It supposed to instanciated only
	once during the program, never copied or assigned.
*/

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


/*
						.------.
						| init |
						'------'
*/

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


/*
					.---------------.
					| cleanup (cgi) |
					'---------------'
*/

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

/*
		So this might be the most important part of the project, and
	it is where we use epoll (or poll or select) to implement multi-
	plexing. Once epoll is initialized and configured we can use
	epoll_wait to manage a bunch of file descriptors: some of them
	refers to listening sockets (the one specified in the conf file),
	others refer to the actual connections we opened with our clients.
		Every loop we get all fds that are ready to read and/or write on.
	We then choose what we need to do with EVERY SINGLE ONE of the 
	connections, even if it's not much. The key is to never do long
	operations. Every read and write must be performed once with a 
	reasonable buffer size, not put in a write loop or a read loop.
		This system also allows to use CGI (basically execute another
	script/program to generate the response): we might not know when
	this other process will be done, but it does not matter because 
	we can continue to loop and serve other clients.
		Finally there it a timeout system to disconnect inactive users.
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
				if (_clients[fd].add_data() > 0)
					disconnect_socket(fd, ptr, "Read error.");
			}
			else if (ev & EPOLLERR)
			{
				assert(is_connected(fd), "disconnect/ could not find fd");
				int       error = 0;
				socklen_t errlen = sizeof(error);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0 )
					disconnect_socket(fd, ptr, strerror(error));
				else
					disconnect_socket(fd, ptr, "EPOLLERR");
			}
		}
		/*
			Disconnecting every Clients that timed_out.
		*/
		for (Expire_iterator ex = _expire.begin(); ex != _expire.end() && (*ex)->expire() < time_in_ms(); ex = _expire.begin())
    		{ disconnect_socket_ex(ex); }
	}
}