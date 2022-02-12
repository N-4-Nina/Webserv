#include "../include/Response.hpp"

Response::Response(void)
{
}

// Response::Response(const Response &ref)
// {
// }

// Response	&Response::operator=(const Response &ref)
// {
// 	return (*this);
// }

Response::~Response(void)
{
}

Response::Response(Request req)
{
	(void)req;
	(void)_fd;
	(void)_body;
	std::ifstream       page;
    std::stringstream   buf;
	char				buff[MAXREAD+1];
	memset(buff, 0, MAXREAD + 1);
	page.open ("./website/home.html", std::ifstream::in);
    buf << page.rdbuf();
	//buf.str().c_str();
	//snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 \r\n\r\n<!OKDOCTYPE html>\n<head>\n</head>\n<body>\n<div>Hello There :)</div>\n<img src=\"image.jpg\"/>\n</body>\n</html>");
    write(_fd, buf.str().c_str(), buf.str().size());
}


void			Response::set_status(unsigned int s)
{
	_status = s;
}

unsigned int	Response::status()
{
	return (_status);
}

void			Response::send()
{

}