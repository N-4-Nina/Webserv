#pragma once

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <map>
#include <netdb.h>
#include <algorithm>
#include <vector>
#include <list>

typedef std::string             str_t;
typedef std::pair<str_t, str_t> strPair;
typedef	std::map<str_t, str_t>	strMap;
typedef struct epoll_event      event_t;
typedef struct addrinfo         adrinfo_t;


void    fatal(str_t str);

#define CRLF            "\r\n"
#define SERVER_VERSION  "HTTP/1.1"