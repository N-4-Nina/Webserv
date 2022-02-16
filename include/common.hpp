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
#include <sstream>
#include <string>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>


typedef std::string             str_t;
typedef std::pair<str_t, str_t> strPair;
typedef	std::map<str_t, str_t>	strMap;

// typedef std::pair<int, time_t>  expiry;
// typedef std::list<expiry>		expiryList;
//typedef expiryList::iterator    expiryIt;

typedef std::vector<Client>		client_v;

typedef struct epoll_event      event_t;
typedef struct addrinfo         adrinfo_t;

time_t	time_in_ms(void);

void    fatal(str_t str);
void    assert(bool scal, str_t log);

#define CRLF            "\r\n"
#define SERVER_VERSION  "HTTP/1.1"
#define MAXREAD			4096