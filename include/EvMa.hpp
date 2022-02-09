#ifndef EVMA_HPP
# define EVMA_HPP

#include "common.hpp"

class EvMa
{
	public:
		EvMa(const char *port = "8000", int max_event = 1024);
		//EvMa(const EvMa &ref);				a implementer
		//EvMa	&operator=(const EvMa &ref);
		~EvMa(void);

		void	loop();
	private:
		int		_portNb;
		//int		_socket;
		int		_socket_fd;
		int		_epoll_fd;
		int		_max_event;
		int		_event_nb;

		expiryList	_timeouts;
		event_t		_event;	//rename it _tmp_event
		event_t		*_events;

		void	init_socket( );
		int		unlock_socket(int fd);
		void	init_epoll();
		void	add_to_interest(int fd);
		void	incoming_connections();
		int		read_data(int i);
		int		timeout();
		void	update_expiry(int fd);
		expiryIt	disconnect_socket(expiryIt expired);
};

#endif