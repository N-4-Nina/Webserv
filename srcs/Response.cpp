#include "Response.hpp"
#include "Client.hpp"
#include "str_manips.hpp"
#include "find_nocase.hpp"
#include "EvMa.hpp"
#include <fstream>

Response::Response(void)
{
}

Response::Response(const Response &ref)
{
	_cgi = ref._cgi;
	_client = ref._client;
	_conf = ref._conf;
	_route = ref._route;
	_flags = ref._flags;
	_loc = ref._loc;
	_status = ref._status;
	_fd = ref._fd;
	_headers = ref._headers;
	_index = ref._index;
	_head = ref._head;
	_body = ref._body;
}

Response	&Response::operator=(const Response &ref)
{
	if (&ref != this)
	{
		_cgi = ref._cgi;
		_client = ref._client;
		_conf = ref._conf;
		_route = ref._route;
		_flags = ref._flags;
		_loc = ref._loc;
		_status = ref._status;
		_fd = ref._fd;
		_headers = ref._headers;
		_index = ref._index;
		_head = ref._head;
		_body = ref._body;
	}
	return (*this);
}

Response::~Response(void)
{
}

Response::Response(Request &req, Config *conf, Client *client, EvMa *evma) : _cgi(evma), _conf(conf), _flags(0), _fd(req.fd())
{
	_client = client;
	_flags |= RES_READY;
	_flags |= RES_STARTED;

	if (req.headers().count("connection") && req.headers()["connection"] == "close")
		_flags |= RES_CLOSE;
	if (req.isBad())
	{
		set_status(req.error());
		return ;
	}
	else
		{ _status = 200; }

	select_location(req);
	if (_flags & RES_LOCATED)
	{
		if ((_loc->flags() & LOC_METHOD))
		{
			if (!(req.type() & _loc->methods()))
			{
				set_status(403);
				return;
			}
		}
		if ((_loc->flags() & LOC_AUTO))
		{
			if (get_autoindex(req, false) == 1)
			{
				if (_status < 200 || _status > 299)
					get_error_page();
				return;
			}
		}
		cgi_match(req._ressource);
		if (_flags & RES_ISCGI)
		{
			_flags &= ~RES_READY;
			set_body_cgi(req);
		}
		if (req.type() == POST && (_loc->flags() & LOC_UPLOAD))			//please note that in this state we cannot upload on the default route. this is intentional.
			upload_file(req);
		if (_loc->flags() & LOC_REDIR)
		{
			set_status(_loc->redir().first);
			set_redir();
			return;
		}
	}
	if (_conf->autoindex() == "on")
		{
			if (get_autoindex(req, true) == 1)
			{
				if (_status < 200 || _status > 299)
					get_error_page();
				return;
			}
		}
	if (!(_flags & RES_ISCGI))
		set_body_ress(req, conf);
	//if (_status < 200 || _status > 299)
	//	get_error_page();
}

void			Response::set_status(unsigned int s)
{
	_status = s;
	if (_status < 200 || _status > 299)
		get_error_page();
}

void			Response::add_header(str_t key, str_t val)
{
	_headers[key] = val;
}

void			Response::set_redir()
{
	add_header("location", _loc->redir().second);
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
				path = root + _index;	//i think it's just this because at this point we checked that root was INCLUDED AT THE START of ressource.
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
			str_t tmp = conf->root() + *lit; // Soooooo... shouldn't always be conf->root there...
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
		path = conf->root().substr(0, conf->root().size() - 1) + req._ressource;

		struct stat s;
		if ( stat(path.c_str(), &s) == 0 && ( s.st_mode & S_IFDIR ))
		{
			std::list<str_t>::iterator it = _conf->index().begin();
			for (; it != _conf->index().end(); it++)
			{
				str_t file = path + "/" + *it;
				if (!access(file.c_str(), 0))
				{
					path = file;
					break;
				}
			}
			if (it == _conf->index().end())
				set_status(404);
		}
	}
	if (_status == 404)
		return ;
	
	std::ifstream       page;
    std::stringstream   buf;
	page.open (path.c_str(), std::ifstream::in);

	if (!page.is_open())
	{
		set_status(404);
		log(_client->_serv, _client, "Could not open ressource file.");
	}
	else
    {
		buf << page.rdbuf();
		_body = buf.str();
		set_headers(path, req);
	}
}

void	Response::set_headers(str_t path, Request &req)
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
	if (_body.size())
		add_header("content-length", to_string<size_t>(_body.size() + 1));

	//check if request got connection:close specified, else:
	if (req.headers().count("connection") && req.headers()["connection"] == "close")
		add_header("Connection", "close");
	else
		add_header("Connection", "keep-alive");
}

unsigned int	Response::status()
{ return (_status); }

strMap			Response::headers()
{ return (_headers); }

FLAGS			Response::flags()
{ return (_flags); }

CGI				&Response::cgi()
{ return (_cgi); }

void			Response::get_error_page()
{
	if (_conf->error_page().count(to_string<unsigned int>(_status)))
	{
		str_t			filepath = _conf->root() + _conf->error_page()[to_string<unsigned int>(_status)];
		std::ifstream	stream(filepath.c_str());
		std::stringstream buffer;
		std::vector<raw_str_t>::iterator	it;

		if (stream.is_open())
		{
			buffer << stream.rdbuf();
			_body = buffer.str();
			stream.close();
			if (_body.size())
			{
				add_header("content-length", to_string(_body.size() + 1));
				return;
			}
		}
	}
	_body = _error_page[0] + to_string(_status) + " : " + _codes[_status] + _error_page[1];
	add_header("content-length", to_string(_body.size() + 1));
}

bool			Response::cgi_match(str_t uri)
{
	if ( _loc->route() == "/cgi")
	{
		if (uri == "/cgi/" || uri == "/cgi")
			_flags |= RES_DEFCGI;
		_flags |= RES_ISCGI;
		return true;
	}
	return false;
	// size_t dotPos;
	// if (uri.size() < _loc->cgi_extension().size() || (dotPos = uri.find(".")) == uri.npos)
	// 	return false;
	// if (uri.substr(dotPos, str_t::npos) == _loc->cgi_extension() && _loc->route() == "/cgi")
	// {
	// 	_flags |= RES_ISCGI;
	// 	return true;
	// }
	return false;
}

void			Response::select_location(Request &req)
{
	//location_v loc = _conf->location();
	for (location_v::iterator it = _conf->location().begin(); it != _conf->location().end(); it++)
	{
		if (req._ressource.find(it->route()) != 0)
			continue;
		else if (req._ressource.length() == it->route().length() || req._ressource[it->route().length()] == '/')
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

	str_t	tmp =_conf->root() + req._ressource;
	
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
	buffer += CRLF;
	for (strMap::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		buffer += it->first;
		buffer += ":";
		buffer += it->second;
		buffer += CRLF;
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
	
	str_t			filename;
	
	std::vector<raw_str_t>::iterator it = req.body().begin();
	for (; it < req.body().end(); it++)
	{
		filename.clear();
		for (; it < req.body().end() && it->size() != 0; it++)
		{
			raw_str_t::iterator pos_cd;
			if ((pos_cd = raw_find(*it, "Content-Disposition", 19)) != it->end())		// an equivalent to find nocase would be better
			{	
				raw_str_t::iterator pos_fn = raw_find(*it, "filename=", 9);
				//raw_str_t::iterator itStr = it->begin() + pos_fn + 1;
				pos_fn++;
				for (; pos_fn != it->end() && *pos_fn != '\"'; pos_fn++);
				pos_fn++;
				for (; pos_fn != it->end() && *pos_fn != '\"'; pos_fn++)
					{ filename.append(1, *pos_fn); }
				//break;
			}
		}
		if (filename == "")
			filename = "default_upload_name.raw";
		str_t			filepath = _loc->upload_path() + filename;
		std::ofstream	stream(filepath.c_str(), std::ofstream::binary);

		it++;			//boundaries-"headers" are precedeed and followed by empty line
		for (; it < req.body().end() && !req.isBoundary(*it); it++)
		{
			char *tmp = raw_to_char(*it);
			stream.write(tmp, it->size());
			stream.write(CRLF, 2);
		}
		stream.close();
	}
}

void			Response::send()
{
	str_t  res = add_head();
	res += _body + "\4";
	const char *tmp = res.c_str();
	write(_fd, tmp, res.size());
	fsync(_fd);
}

void			Response::reset()
{
	_route.clear();
	_flags = 0;
	_loc = NULL;
	_status = 0;
	_headers.clear();
	_index.clear();
	_head.clear();
	_body.clear();
	_cgi.reset();
}

void	Response::check_cgi()
{
	_cgi.check(&_flags, &_status);
	if ((_flags & RES_READY) && (_status < 200 || _status > 299))
		get_error_page();
	else if ((_flags & RES_READY))
	{
		_body = _cgi.body();
		add_header("content-length", to_string<size_t>(_body.size() + 1));
		add_header("content-type", "text/html");
	}
}

void	Response::kill_cgi()
{
	kill(_cgi.pid(), SIGABRT);
	_cgi.reset();
}

void	Response::set_body_cgi(Request req)
{
	char tmp[256];
	location_v loc = _conf->location();
	size_t i = 0;
	for (location_v::iterator it = loc.begin() ; it != loc.end() ; ++it, ++i)
	{
		if (!loc.at(i).cgi_path().empty())
		{
			_cgi.set_binary(loc.at(i).cgi_path());
		}
	}
	
	getcwd(tmp, 256);
	str_t target = tmp;
	target.append("/www/cgi/");
	if (_flags & RES_DEFCGI)
	{
		for (location_v::iterator it = _conf->location().begin(); it != _conf->location().end(); it++)
		{
			_loc = &(*it);
			if (req._ressource.find(it->route()) != 0)
				continue;
			else
			{
				target.append(*it->index().begin());
				if (access( target.c_str(), F_OK ))
				{
					_status = 404;
					get_error_page();		
					return;
				}
				break;
			}
		}
	}
	else
	{
		size_t found = req._ressource.find_last_of("/");
		target.append(req._ressource.substr(found + 1));
		if (access( target.c_str(), F_OK ))
		{
			_status = 404;
			get_error_page();		
			return;
		}
	}
	_cgi.set_script_name(target.substr(target.find("/cgi")));
	_cgi.exec_cgi(target, req, this->headers(), &_flags, &_status);

	_body = _cgi.body();
	add_header("content-length", to_string<size_t>(_body.size() + 1));		//move maybe ? at least cl
	add_header("content-type", "text/html");
	add_header("Connection", "keep-alive");
}

int		Response::get_autoindex(Request req, bool home_page)
{
	std::stringstream	buffer;
	str_t	path;
	if (home_page == false)
		path = _loc->root();
	else
		path = _conf->root();
	int	port;
	str_t	server_name;

	for (strMap::iterator it = req.headers().begin() ; it != req.headers().end() ; ++it)
	{
		if (it->first == "host")
		{
			size_t points = it->second.find(":");
			size_t start = 0;
			size_t end = it->second.size() - 1;
			while (start != points)
				start++;
			server_name = it->second.substr(0, start);
			port = strtol(it->second.substr(points + 1, end).c_str(), NULL, 10);
		}
	}
	int dot = 0; 

	if ((dot = req._ressource.find(".")) != -1)
	{	str_t ext = req._ressource.substr(dot, str_t::npos);
		if (_mimeTypes.count(ext))
		{
			return 0;
		}
	}
	std::cout << req._ressource << " " << path << " " << server_name << " " << port << std::endl;
	buffer << Autoindex::getPage(req._ressource.c_str(), path.c_str(), server_name, port); // nom du serveur qu'on ne gere pas
	// + auto index front page
	_body = buffer.str();
	add_header("content-length", to_string<size_t>(_body.size() + 1));
	add_header("content-type", "text/html");
	add_header("Connection", "keep-alive");
	return 1;
}