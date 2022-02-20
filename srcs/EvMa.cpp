#include "EvMa.hpp"
#include "Request.hpp"
#include "Client.hpp"

// a terme devra prendre une ref sur config (et le garder en tant qu'attribut)
EvMa::EvMa(Config &conf)
{
	/* should put all of this in init list -- also put missing stuff (nb_events..)*/

	//_port = strdup(port);	// duplicate because default value
	_portNb = conf.port()[0];		//warning = should implement multiple listening socket
	_max_event = conf.client_max();
	_event_nb = 0;

	_clients.reserve(_max_event);
	init_socket();			//so maybe private ??
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

int	EvMa::unlock_socket(int fd)
{
	int flags;
	flags = fcntl(fd,  F_GETFL);
	//if (flags == -1)
	//	handle_error
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		fatal("unlock socket failed");
	return (0);
}

void	EvMa::init_socket()
{
	sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(_portNb);

	if ((_socket_fd = socket(servaddr.sin_family, SOCK_STREAM, 0)) < 0)
	{
		std::cout << errno;
        fatal("could not create socket");
	}


	int optionValue = 1;
    setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));

	if ((bind(_socket_fd , (sockaddr*) &servaddr, sizeof(servaddr))) < 0)
        fatal("could not bind");
    
    if (listen(_socket_fd , 10) < 0)
		fatal("can not listen (i am a cisgender man)");


	unlock_socket(_socket_fd);
}

void	EvMa::init_epoll()
{
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		fatal("epoll creation failed");

	_events = static_cast<event_t*>(calloc(_max_event, sizeof(event_t)));

	_event.data.fd = _socket_fd;
	_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl (_epoll_fd, EPOLL_CTL_ADD, _socket_fd, &_event) == -1) /*intialize interest list*/
		fatal("epoll ctl");
}

// void enable_keepalive(int sock) {
//     int yes = 1;
//     setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

//     int idle = 1;
//     setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

//     int interval = 1;
//     setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

//     int maxpkt = 10;
//     setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
// }

void	EvMa::add_to_interest(int fd)
{
	unlock_socket(fd);
	//enable_keepalive(fd);
	_event.data.fd = fd;
	_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_event) == -1)
		fatal("failed to add incoming connection to interest list.");
	//expiry ex = std::make_pair(fd, time_in_ms() + 5000);
	//_clients.push_back(ex);
	_clients.push_back(Client(fd));
	(_clients.end()-1)->touch();
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

void	EvMa::incoming_connections()
{
	int fd;
	struct sockaddr     incoming;		//should probably be before loop start for opti but for now it's fine
    socklen_t			incSize;

	for (;;)
	{
		fd = accept(_socket_fd, &incoming, &incSize); //we accept with socket fd as we listen on this on
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
			add_to_interest(fd);
	}
}

void	EvMa::update_expiry(int fd)
{

	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].fd() == fd)
		{
			_clients[i].touch();
			_clients.push_back(_clients[i]);
			_clients.erase(_clients.begin() + (i - 1));
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
	return (_clients[0]);
}

int	EvMa::read_data(int i)
{
	//int 			n;
	Client			&client = find_by_fd(_events[i].data.fd);

	update_expiry(i);
	if (client.add_data())
		client.respond();

    // if (n <= 0)
    // {
	// 			//not sure what to do here.. for now my guess is actually respond.
	// 	//fatal("read error");
	// 	//std::cout << "read error (cannot check errno right now)" << std::endl;
	// 	client.respond();
	// }


    //std::cout << input;
	//Request			req(input, _events[i].data.fd);
    //req.parse(input);	//now called in constructor
	//req.response();	//might be a bad idea. maybe the response object should be declared here.
					// It mainly depends on what infos we need to respond (spoiler: we probably need a lot.)
	//for testing/ cohesion purposes, i copied this ugly thing:


	//send(_events[i].data.fd, buff, strlen(buff), MSG_DONTWAIT); 
	//			subject says we can use any of those two ..
	//int numbytes;
	// if ((numbytes = recv(_socket_fd, buff, MAXREAD, MSG_DONTWAIT)) == 0)
	// {
	// 	std::cout << "client shutdown connection;";
	// 	close(_events[i].data.fd);
	// }
	//fsync(_events[i].data.fd);						//this is a "flush". since we dont always close the ssocket right now, the data are not actually sent.
												// this clears the buffer and force the data to be sent.
	//if (req.headers().count("connection") && req.headers()["connection"] == "close")
	//	break;
	//close(_events[i].data.fd);
	return (0);
}

int	EvMa::write_data(int i)
{
	Client			&client = _clients[_events[i].data.fd];

	client.respond();
	return (0);
}

int	EvMa::timeout()
{
	if (!_clients.size())				//we do not have any open connections and don't need any timeout
		return (-1);
	int to = _clients.begin()->expire() - time_in_ms();
	if (to > 0)
		return (to);
	return (-1);
}

client_v::iterator	EvMa::disconnect_socket(client_v::iterator expired)
{
	if (expired->fd() == 0)
		return (expired);
	std::cout << "closed connection to socket nb " << expired->fd() << std::endl;
	close(expired->fd());
	//epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, expired->first, NULL);
	client_v::iterator tmp = expired;
	expired++;
	_clients.erase(tmp);
	return (expired);
}

void	EvMa::loop()
{
	for (;;)
	{
		_event_nb = epoll_wait(_epoll_fd, _events, _max_event, timeout()); //-1 for timeout means it will block unedfinitely. check if that's the behaviour we want.
		std::cout << "event_nb = "<<  _event_nb << "\n";
		for (int i = 0; i < _event_nb; i++)
		{
			int fd = _events[i].data.fd;
			uint32_t ev = _events->events;
			//HANDLE ERROR WITH & BITWISE OP (why tho). if error, continue
			if (fd == _socket_fd)
				incoming_connections();
			else if (ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				assert(is_connected(fd), "disconnect/ could not find fd");
				disconnect_socket(_clients.begin() + (i - 1));
			}
			else if (ev & EPOLLIN)
			{
				assert(is_connected(fd), "read/ could not find fd");
				read_data(i);
			}
			else if (ev & EPOLLOUT)
			{
				assert(is_connected(fd), "write/ could not find fd");
				write_data(i);
			}
		}
		if (!_clients.size())
			continue;
		//_event_nb = 0;
		for (client_v::iterator ex = _clients.begin(); ex != _clients.end() && ex->expire() < time_in_ms(); ex++)
    		ex = disconnect_socket(ex);
		
	}
}