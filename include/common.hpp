#pragma once

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
#include <algorithm>

typedef std::string             str_t;
typedef std::pair<str_t, str_t> strPair;
typedef	std::map<str_t, str_t>	strMap;

#define CRLF            "\r\n"
#define SERVER_VERSION  "HTTP/1.1"