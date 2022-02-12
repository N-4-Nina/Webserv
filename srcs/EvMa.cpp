#include "EvMa.hpp"
#include "Request.hpp"

// a terme devra prendre un pointeur sur config (et le garder en tant qu'attribut)
EvMa::EvMa(const char *port, int max_event)
{
	/* should put all of this in init list -- also put missing stuff (nb_events..)*/

	//_port = strdup(port);	// duplicate because default value
	_portNb = atoi(port);
	_max_event = max_event;
	_event_nb = 0;

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
		//handle_error
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
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

void	EvMa::add_to_interest(int fd)
{
	unlock_socket(fd);
	_event.data.fd = fd;
	_event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_event) == -1)
		fatal("failed to add incoming connection to interest list.");
	expiry ex = std::make_pair(fd, time_in_ms() + 0);
	_timeouts.push_back(ex);
	std::cout << "added connection. fd is: " << fd << std::endl;
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
	expiryIt it = _timeouts.begin(), ite = _timeouts.end();
	for (; it != ite; it++)
	{
		if (it->first == fd)
		{
			_timeouts.erase(it);
			_timeouts.push_back(std::make_pair(fd, time_in_ms() + 0));
			return ;
		}
	}
}

int	EvMa::read_data(int i)
{
	char            recvline[MAXREAD+1];
	str_t			input;
	int 			n = 1;

	while (n > 0)
	{
		update_expiry(i);
		memset(recvline, 0, MAXREAD+1);
		while ((n = read(_events[i].data.fd, recvline, MAXREAD-1)) >  0)
    	{
    	    input = input + str_t(recvline);
    	    if (recvline[n-1] == '\n')
    	        break ;
			//memset(recvline, 0, MAXREAD+1);
    	}
    	if (n < 0)
    	    {fatal("read error");}
    	//std::cout << input;
		Request			req(input, _events[i].data.fd);
    	//req.parse(input);	//now called in constructor
		//req.response();	//might be a bad idea. maybe the response object should be declared here.
						// It mainly depends on what infos we need to respond (spoiler: we probably need a lot.)



		//for testing/ cohesion purposes, i copied this ugly thing:

		std::ifstream       page;
    	std::stringstream   buf;
		char				buff[MAXREAD+1];

		memset(buff, 0, MAXREAD + 1);
		page.open ("./website/home.html", std::ifstream::in);
    	buf << page.rdbuf();
    	//const std::string& tmp = buf.str();
    	//const char* cstr = tmp.c_str();

		snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 \r\n\r\n<!OKDOCTYPE html>\n<head>\n</head>\n<body>\n<div>Hello There :)</div>\n<img src=\"image.jpg\"/>\n</body>\n</html>");

    	write(_events[i].data.fd, buff, strlen(buff));
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

		if (req.headers().count("connection") && req.headers()["connection"] == "close")
			break;
	}
	close(_events[i].data.fd);
	return (0); 

}

int	EvMa::timeout()
{
	if (!_timeouts.size())				//we do not have any open connections and don't need any timeout
		return (-1);
	int to = _timeouts.begin()->second  -  time_in_ms();
	if (to > 0)
		return (to);
	return (-1);
}

expiryIt	EvMa::disconnect_socket(expiryIt expired)
{
	std::cout << "closed connection to socket nb " << expired->first << std::endl;
	close(expired->second);
	expiryIt tmp = expired;
	expired++;
	_timeouts.erase(tmp);
	return (expired);
}

void	EvMa::loop()
{
	for (;;)
	{
		_event_nb = epoll_wait(_epoll_fd, _events, _max_event, timeout()); //-1 for timeout means it will block unedfinitely. check if that's the behaviour we want.
		for (int i = 0; i < _event_nb; i++)
		{
			//HANDLE ERROR WITH & BITWISE OP (why tho). if error, continue
			if (_events[i].data.fd == _socket_fd)
				incoming_connections();
			else
				read_data(i);
		}
		if (!_timeouts.size())
			continue;
		_event_nb = 0;
		for (expiryIt ex = _timeouts.begin(); ex != _timeouts.end() && ex->second < time_in_ms(); ex++)
    		ex = disconnect_socket(ex);
		
	}
}