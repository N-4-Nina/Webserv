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

Response::Response(Request &req, Config *conf) : _conf(conf), _flags(0), _fd(req.fd())
{
	_status = 200;
	select_location(req);
	if (_flags & RES_LOCATED)
		cgi_match(req._ressource);
	//if (_flags & RES_ISCGI)
	//	set_body_cgi()
	//else
	if (!(_flags & RES_ISCGI))
		set_body_ress(req, conf);
}

void			Response::set_status(unsigned int s)
{
	_status = s;
}

// bool			Response::find_ressource()
// {
// 	str_t filename;
// 	if (_flags & RES_ISINDEX)
// 	{
// 		filename = 
// 	}
// }

void			Response::set_body_ress(Request &req, Config *conf)
{
	str_t path;
	str_t filename;
	if (_flags & RES_LOCATED && _loc->root() != "")
	{
		str_t root = _loc->root();
		if (_flags & RES_ISINDEX)
		{
			if (_flags & RES_INDEXDEF)
				path = _loc->root() + _index;	//i think it's just this because at this point we check that root was INCLUDED AT THE START of ressource.
			else
			{
				for (std::list<str_t>::iterator lit = _loc->index().begin(); lit != _loc->index().end(); lit++)
				{
					str_t tmp = _loc->root() + *lit;
					if (!access( tmp.c_str(), F_OK ))
					{
						path = tmp;
						break;
					}
				}
				set_status(404);
			}
		}
		else
			path = _loc->root() + req._ressource.substr(_loc->route().size(), req._ressource.npos);
	}
	else
	{
		str_t root = conf->root();
		path = conf->root() + req._ressource;
	}
	std::ifstream       page;
    std::stringstream   buf;
	page.open (path.c_str(), std::ifstream::in);
	if (!page.is_open())
	{
		set_status(404);
		std::cout << "piou404\n";
	}
	
    buf << path;
	std::cout << buf.str();
	_body = buf.str();
}

unsigned int	Response::status()
{
	return (_status);
}

bool			Response::cgi_match(str_t uri)
{
	if (uri.size() < _loc->cgi_extension().size())
		return false;
	if (uri.find(_loc->cgi_extension(), uri.size() - _loc->cgi_extension().size()))
	{
		_flags |= RES_ISCGI;
		return true;
	}
	return false;
}

void			Response::select_location(Request &req)
{
	location_v loc = _conf->location();
	for (location_v::iterator it = loc.begin(); it != loc.end(); it++)
	{
		if (req._ressource.find(it->route()) != 0)
			continue;
		else
		{
			_flags |= RES_LOCATED;
			_loc = &(*it);							//don't judge me ok
			if (req._ressource.size() == it->route().size())
			{
				_flags |= RES_ISINDEX;
				return ;
			}
			for (std::list<str_t>::iterator lit = it->index().begin(); lit != it->index().end(); lit++)
			{
				if (req._ressource.find(*lit, it->route().size()) != lit->npos)
				{
					_flags |= RES_ISINDEX;
					_flags |= RES_INDEXDEF;
					_index = *lit;
				}
			}
			return;
		}
	}
	//after this we come back to the default (server block)
	for (std::list<str_t>::const_iterator lit = _conf->index().begin(); lit != _conf->index().end(); lit++)
	{
		if (req._ressource.find(*lit) == 0)
		{
			_flags |= RES_ISINDEX;
			_flags |= RES_INDEXDEF;
			_index = *lit;
		}
	}
}

void			Response::write_head()
{
	char statusbuf[4];

	write(_fd, "HTTP/1.1 ", 9);
	sprintf(statusbuf, "%d", _status);
	write(_fd, statusbuf, 3);
	//write other headers
	write(_fd, "\n\r\n\r", 4);
}

void			Response::write_body()
{
	write(_fd, _body.c_str(), _body.size());
}

void			Response::send()
{
	(void)_fd;
	(void)_body;
	std::cout << "bye\n";
	write_head();
	write_body();
	std::cout << "hello\n";
}