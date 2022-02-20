#ifndef EVMA_HPP
# define EVMA_HPP

#include "common.hpp"
#include "Client.hpp"
#include "Config.hpp"

class EvMa
{
	public:
		EvMa(Config &conf);
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

		//expiryList			_timeouts;
		event_t				_event;	//rename it _tmp_event
		event_t				*_events;
		std::vector<Client>	_clients;

		void	init_socket( );
		int		unlock_socket(int fd);
		void	init_epoll();
		void	add_to_interest(int fd);
		void	incoming_connections();
		int		read_data(int i);
		int		write_data(int i);
		int		timeout();
		Client	&find_by_fd(int fd);
		void	update_expiry(int fd);
		bool	is_connected(int fd);
		client_v::iterator	disconnect_socket(client_v::iterator expired);
};

#endif