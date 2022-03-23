#include "common.hpp"
#include "Request.hpp"
#include "EvMa.hpp"
#include "Config.hpp"

#define DEFAULT_PORT	8002
#define MAX_EVENTS		5

void initialize_error_pages();

void    fatal(str_t str)
{
    std::cerr << str << "\n";
    exit(EXIT_FAILURE);
}

void    assert(bool scal, str_t log)
{
    if (!scal)
        fatal(log);
}


int main(int argc, char **argv)
{
    strMap          req_headers;

    std::vector<Config> conf = parsing_config(argc, argv);
    std::cout << conf[0];
    EvMa	ev(conf);
    initialize_mime_types();
    initialize_error_pages();
    ev.loop();
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