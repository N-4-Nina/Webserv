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
	//if (_flags & RES_LOCATED)
	//	cgi_match(req._ressource);
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

void			Response::add_header(str_t key, str_t val)
{
	_headers[key] = val;
}

void			Response::set_body_ress(Request &req, Config *conf)
{
	str_t path;
	str_t filename;
	if (_flags & RES_LOCATED)
	{
		str_t root;
		if (_loc->root() != "")
			root = _loc->root();
		else
			root = conf->root();
		if (_flags & RES_ISINDEX)
		{
			if (_flags & RES_INDEXDEF)
				path = root + _index;	//i think it's just this because at this point we check that root was INCLUDED AT THE START of ressource.
			else
			{
				for (std::list<str_t>::iterator lit = _loc->index().begin(); lit != _loc->index().end(); lit++)
				{
					str_t tmp = root + *lit;
					if (!access( tmp.c_str(), F_OK ))
					{
						path = tmp;
						break;
					}
					else
						set_status(404);
				}		
			}
		}
		else
			path = _loc->root() + req._ressource.substr(_loc->route().size(), req._ressource.npos);
	}
	else if (_flags & RES_ISINDEX)
	{
		for (std::list<str_t>::iterator lit = conf->index().begin(); lit != conf->index().end(); lit++)
		{
			str_t tmp = conf->root() + *lit;
			std::cout << tmp  << std::endl;
			if (!access( tmp.c_str(), F_OK ))
			{
				path = tmp;
				break;
			}
			else
				set_status(404);
		}	
	}
	else
	{
		path = conf->root() + req._ressource;
		if (!access(path.c_str(), F_OK))
			add_header("content-type", "image/jpeg");
		else
			set_status(404);
	}
	std::ifstream       page;
    std::stringstream   buf;
	page.open (path.c_str(), std::ifstream::in);
	if (!page.is_open())
	{
		set_status(404);
		std::cout << "piou404\n";
	}
	else
    {
		buf << page.rdbuf();
		std::cout << path;
		std::cout << _body;
		//_body << page;
		//std::cout << buf.str();
		_body = buf.str();
	}
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
	//location_v loc = _conf->location();
	for (location_v::iterator it = _conf->location().begin(); it != _conf->location().end(); it++)
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
	if (req._ressource == "/")
	{
		_flags |= RES_ISINDEX;
		return ;
	}
	for (std::list<str_t>::iterator lit = _conf->index().begin(); lit != _conf->index().end(); lit++)
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

	statusbuf[3] =0;
	write(_fd, "HTTP/1.1 ", 9);
	sprintf(statusbuf, "%d", _status);
	write(_fd, statusbuf, 3);
	write(_fd, "\n", 1);

	for (strMap::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		write(_fd, it->first.c_str(), it->first.size());
		write(_fd, ":", 1);
		write(_fd, it->second.c_str(), it->second.size());
		write(_fd, "\n", 1);
	}

	if (_headers.size() == 0)
		write(_fd, "Content-Type: text/html\n", 25);
	//write(_fd, "Transfer-Encoding: deflate\n", 24);
	//write(_fd, "Content-length: 141\n", 39);
	write(_fd, statusbuf, 3);
	write(_fd, "\n\r\n\r", 4);
}

void			Response::write_body()
{
	write(_fd, _body.c_str(), _body.size());
}

void			Response::send()
{
	write_head();
	write_body();

}