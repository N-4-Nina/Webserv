#include "../include/common.hpp"
#include "../include/Request.hpp"
#include "EvMa.hpp"

#define MAXLINE			4096
#define DEFAULT_PORT	8002
#define MAX_EVENTS		5

void    fatal(str_t str)
{
    std::cerr << str << "\n";
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    int port = DEFAULT_PORT;
    
    char            buff[MAXLINE+1];
    char            recvline[MAXLINE+1];
    strMap          req_headers;

	EvMa	ev;


	if (argc == 2)
        port = atoi(argv[1]); //using user defined port

    


	
    
    // for (;;)
    // {
    //     Request req;
    //     str_t   input;
    //     struct sockaddr_in  addr;
    //     struct sockaddr     incoming;
    //     socklen_t addr_len, incSize;
    //     std::ifstream       page;
    //     std::stringstream   buf;

    //     std::cout << "Polling for input " << port << "\n";
	// 	event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000); //should check for timeout right after if set ?? maybe
	// 	std::cout << event_count << " ready events\n";

	// 	for (int i = 0; i < event_count; i++)
	// 	{
    //     	connfd = accept(listenfd, &incoming, &incSize);
    //     	memset(recvline, 0, MAXLINE);

    //     	while ((n = read(connfd, recvline, MAXLINE-1)) >  0)
    //     	{
    //     	    input = input + str_t(recvline);
    //     	    if (recvline[n-1] == '\n')
    //     	        break ;
    //     	}
    //     	if (n < 0)
    //     	    fatal("read error");
    //     	std::cout << input;
    //     	req.parse(input);
    //     	snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 \r\n\r\n<!OKDOCTYPE html>\n<head>\n</head>\n<body>\n<div>Hello There :)</div>\n<img src=\"image.jpg\"/>\n</body>\n</html>");

    //     	page.open ("./website/home.html", std::ifstream::in);
    //     	buf << page.rdbuf();
    //     	const std::string& tmp = buf.str();   
    //     	const char* cstr = tmp.c_str();
    //     	write(connfd, buff, strlen((char*)buff));
    //     	close(connfd);
	// 	}
    // }
}