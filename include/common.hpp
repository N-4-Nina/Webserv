#pragma once

#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Server;
class Config;
class Client;
class Location;

typedef	unsigned int			FLAGS;

typedef std::string             str_t;
typedef std::pair<str_t, str_t> strPair;
typedef	std::map<str_t, str_t>	strMap;

typedef std::map<int, Server>	Cluster;
typedef	std::map<int, Client>	Clients_pool;
typedef	std::map<int, int>		listen_sockets;

typedef	std::vector<Location>	location_v;
typedef std::vector<Config>		config_v;
typedef std::vector<int>		int_v;

typedef struct epoll_event      event_t;
typedef struct addrinfo         adrinfo_t;

time_t	time_in_ms(void);

void    fatal(str_t str);
void    assert(bool scal, str_t log);
int		unlock_socket(int fd);
void	initialize_mime_types();

#define CRLF            "\r\n"
#define SERVER_VERSION  "HTTP/1.1"
#define MAXREAD			100000
#define MAXCONN			512