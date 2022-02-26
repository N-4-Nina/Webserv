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

Response::Response(Request &req, Config *conf) : _conf(conf), _fd(req.fd())
{
	set_route(req);
}


void			Response::set_status(unsigned int s)
{
	_status = s;
}

unsigned int	Response::status()
{
	return (_status);
}

void			Response::set_route(Request &req)
{
	std::vector<Location> const &Routes = _conf->location();
	for (std::vector<Location>::const_iterator it = Routes.begin(); it != Routes.end(); it++)
	{
		if (it->root() == req.headers()["route"])
		{
			std::cout << "found route in loc\n";
			break;
		}
	}
	

}

void			Response::send()
{
	(void)_fd;
	(void)_body;
	std::ifstream       page;
    std::stringstream   buf;

	page.open ("./www/home.html", std::ifstream::in);
	
    buf << page.rdbuf();

	std::string str = buf.str();

	//std::cout << str;
	
	write(_fd, str.c_str(), str.size());
	//free(buff);
}