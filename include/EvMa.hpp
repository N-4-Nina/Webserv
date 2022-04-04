#ifndef EVMA_HPP
# define EVMA_HPP

#include "common.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "Server.hpp"

class EvMa
{
	public:
		EvMa(config_v &conf);
		//EvMa(const EvMa &ref);				a implementer
		//EvMa	&operator=(const EvMa &ref);
		~EvMa(void);
		void	close_all(void);
		
		void	loop();
	private:
		Cluster			_cluster;
		Clients_pool	_clients;
		Expire_List		_expire;
		int		_epoll_fd;
		int		_event_nb;
		size_t	_cluster_size;
		
		event_t				_event;	//rename it _tmp_event
		event_t				*_events;
		
		void	init_epoll();
		void	add_to_interest(int fd, Server *serv);
		void	update_expiry(int fd);
		void	incoming_connections(int inc_fd, Server *serv);
		bool    is_connected(int fd);
		int		timeout();
		bool	is_listen(int fd, Server **serv);
		void	disconnect_socket(int fd, Server *serv, str_t reason = "unspecified reason.");
		void	disconnect_socket_ex(Expire_iterator ex);

};

#endif