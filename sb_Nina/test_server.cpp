#include "../include/common.hpp"
#include "../include/Request.hpp"

#define MAXLINE 4096
#define DEFAULT_PORT 8002

void    fatal(std::string str)
{
    std::cerr << str << "\n";
    exit(EXIT_FAILURE);
}




int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    int port = DEFAULT_PORT;
    struct sockaddr_in servaddr;
    char            buff[MAXLINE+1];
    char            recvline[MAXLINE+1];
    strMap          req_headers;

    if (argc == 2)
        port = atoi(argv[1]);
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        fatal("could not create socket");
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(listenfd, (sockaddr*) &servaddr, sizeof(servaddr))) < 0)
        fatal("could not bind");
    
    if (listen(listenfd, 10) < 0)
        fatal("can not listen (i am a cisgender man)");
    
    for (;;)
    {
        Request req;
        str_t   input;
        struct sockaddr_in  addr;
        struct sockaddr     incoming;
        socklen_t addr_len, incSize;
        std::ifstream       page;
        std::stringstream   buf;

        std::cout << "Waiting for connections on port " << port << "\n";
        connfd = accept(listenfd, &incoming, &incSize);
        memset(recvline, 0, MAXLINE);

        while ((n = read(connfd, recvline, MAXLINE-1)) >  0)
        {
            input = input + str_t(recvline);
            if (recvline[n-1] == '\n')
                break ;
        }
        if (n < 0)
            fatal("read error");
        std::cout << input;
        req.parse(input);
        snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 OK\r\n\r\n<!DOCTYPE html>\n<head>\n</head>\n<body>\n<div>Hello There :)</div>\n<img src=\"image.jpg\"/>\n</body>\n</html>");

        page.open ("./website/home.html", std::ifstream::in);
        buf << page.rdbuf();
        const std::string& tmp = buf.str();   
        const char* cstr = tmp.c_str();
        write(connfd, buff, strlen((char*)buff));
        close(connfd);
    }
}