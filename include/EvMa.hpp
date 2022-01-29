#ifndef EVMA_HPP
# define EVMA_HPP

#include "common.hpp"

class EvMa
{
	public:
		EvMa(const char *port = "8000", int max_event = 5);
		//EvMa(const EvMa &ref);				a implementer
		//EvMa	&operator=(const EvMa &ref);
		~EvMa(void);

		void	loop();
	private:
		char*	_port;
		int		_portNb;
		//int		_socket;
		int		_socket_fd;
		int		_epoll_fd;
		int		_max_event;
		int		_event_nb;
		event_t	_event;	//rename it _tmp_event
		event_t	*_events;
		std::allocator<event_t> _alloc;

		void	init_socket( );
		int		unlock_socket(int fd);
		void	init_epoll();
		void	add_to_interest(int fd);
		void	incoming_connections();
		int		read_data(int i);
};

#endif