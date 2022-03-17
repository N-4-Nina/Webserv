#include "Response.hpp"
#include "str_manips.hpp"
#include "find_nocase.hpp"

#include <fstream>
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
	if (req.isBad())
	{
		_status = req.error();
		return;
	}
	_status = 200;
	select_location(req);
	if (_flags & RES_LOCATED)
	{
		//	cgi_match(req._ressource);
		//if (_flags & RES_ISCGI)
		//	set_body_cgi()
		if (req.type() == R_POST && (_loc->flags() & LOC_UPLOAD))			//please note that in this state we cannot upload on the default route. this is intentional.
			upload_file(req);
	}
	//else if
	
	if (!(_flags & RES_ISCGI))
		set_body_ress(req, conf);
	if (_status < 200 || _status > 299)
		get_error_page();
	// else							// -> pour tester a la zbeul avec cgi.conf
	//     exceCGI(req);
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
		if (access(path.c_str(), F_OK))
		{
			set_status(404);
			return;
		}
	}
	std::ifstream       page;
    std::stringstream   buf;
	page.open (path.c_str(), std::ifstream::in);
	if (!page.is_open())
	{
		set_status(404);
		std::cout << "Could not open ressource file.\n";
	}
	else
    {
		buf << page.rdbuf();
		std::cout << path;
		std::cout << _body;
		//_body << page;
		//std::cout << buf.str();
		_body = buf.str();
		set_headers(path);
	}
}

void	Response::set_headers(str_t path)
{
	/* Setting content-type */
	size_t point;
	/* Setting default value before proceeding */
	add_header("content-type", _mimeTypes[".txt"]);
	if ((point = path.find(".")) != path.npos)
	{
		str_t ext = path.substr(point, path.npos);
		if (_mimeTypes.count(ext))
			add_header("content-type", _mimeTypes[ext]);
	}

	add_header("content-length", to_string<size_t>(_body.size()));
	// add_header("content-length", "1500");
	// std::cout << "\n\n\n\nCONTENT: " << _body.size() << std::endl;
}

unsigned int	Response::status()
{ return (_status); }

strMap			Response::headers()
{ return (_headers); }

void			Response::get_error_page()
{
	_body = _error_page[0] + to_string(_status) + " : " + _codes[_status];
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

str_t			Response::add_head()
{
	str_t		buffer;

	buffer = "HTTP/1.1 ";
	buffer += to_string<size_t>(_status);
	buffer += "\n";
	for (strMap::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		buffer += it->first;
		buffer += ":";
		buffer += it->second;
		buffer += "\n";
	}
	buffer += CRLF;
	return (buffer);
}


void			Response::upload_file(Request &req)
{
	if (!req.body().size())
	{
		set_status(500);
		return;
	}
	std::ofstream	stream;
	str_t			filename;
	

	std::vector<str_t>::iterator it = req.body().begin();
	for (; it != req.body().end(); it++)
	{
		size_t pos_cd ;
		if ((pos_cd = find_nocase<str_t>(*it, "Content-Disposition")) != it->npos)
		{	
			size_t pos_fn = it->find("filename=");
			str_t::iterator itStr = it->begin() + pos_fn + 1;
			for (; itStr != it->end() && *itStr != '\"'; itStr++);
			itStr++;
			for (; itStr != it->end() && *itStr != '\"'; itStr++)
				filename.append(1, *itStr);
			break;
		} 
	}
	if (filename == "")
		filename = "default_upload_name.raw";
	str_t			filepath = _loc->upload_path() + filename;
	std::cout << filepath <<std::endl;
	stream.open(filepath.c_str());


	for (; it != req.body().end(); it++)
		stream << *it;																					//
	stream.close();
}

void			Response::send()
{
	str_t  res = add_head();
	res += _body;
	write(_fd, res.c_str(), res.size());
}

str_t Response::exceCGI(Request req)
{
	CGI cgi;
	str_t target = getenv("HOME");
	location_v loc = _conf->location();
	size_t i = 0;

	for (location_v::iterator it = loc.begin() ; it != loc.end() ; ++it, ++i)
		if (!loc.at(i).cgi_path().empty())
			cgi.set_binary(loc.at(i).cgi_path());
	
	target.append("/webserv/www/cgi/hello.py");
	cgi.set_script_name(target.substr(target.find("/cgi")));
    cgi.exec_cgi(target, req, this->headers());

	_body = cgi.body();
	return (_body);
}
