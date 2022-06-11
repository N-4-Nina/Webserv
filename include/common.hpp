// #include <dirent.h>
#pragma once

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
# include <sys/wait.h>
#include <unistd.h>
#include <vector>

class Server;
class Config;
class Client;
class Location;

typedef	unsigned long			FLAGS;

typedef std::string             str_t;
typedef std::pair<str_t, str_t> strPair;
typedef	std::map<str_t, str_t>	strMap;

typedef std::map<int, Server>	Cluster;
typedef	std::map<int, Client>	Clients_pool;
typedef	std::map<int, int>		listen_sockets;

typedef std::pair<long, str_t>  Redir;

typedef	std::vector<Location>	location_v;
typedef std::vector<Config>		config_v;
typedef std::vector<int>		int_v;
typedef std::vector<str_t>      str_v;

typedef struct epoll_event      event_t;
typedef struct addrinfo         adrinfo_t;

typedef std::vector<char>       raw_str_t;

#define CRLF            "\r\n"
#define SERVER_VERSION  "HTTP/1.1"
#define MAXREAD			100000
#define MAXCONN			512
#define TIMEOUT         5000