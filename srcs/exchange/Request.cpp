#include "Request.hpp"
#include "str_manips.hpp"
#include "find_nocase.hpp"
#include "flags.hpp"
#include "utils.hpp"

/*
					.--------------.
					| Constructors |
					'--------------'
*/

Request::Request(int fd, Config *conf) : _conf(conf), _fd(fd), _read_body(0)
{
}

Request::Request(unsigned int error, int fd) : _fd(fd), _error(error)
{
	_flags = 0 | REQ_ISBAD;
}

Request::Request(const Request &ref)
{
	_fd = ref._fd;
	_type = ref._type;
	_headers = ref._headers;
	_ressource = ref._ressource;
	_queryParam = ref._queryParam;
	_flags = ref._flags;
	_read_body = ref._read_body;
	_conf = ref._conf;
	_port = ref._port;
}

Request &Request::operator=(const Request &ref)
{
	if (this != &ref)
	{
		_fd = ref._fd;
		_type = ref._type;
		_headers = ref._headers;
		_ressource = ref._ressource;
		_queryParam = ref._queryParam;
		_flags = ref._flags;
		_read_body = ref._read_body;
		_conf = ref._conf;
		_port = ref._port;
	}
	return (*this);
}

Request::~Request(void)
{
}


void Request::reset()
{
	_boundary.clear();
	_type = 0;
	_cl = 0;
	_headers.clear();
	_error = 0;
	_flags = 0;
	
	_read_body = 0;
	_body.clear();
	_queryParam.clear();
	_query_string.clear();
	_ressource.clear();
}

/*
					.-------------------------.
					| Building from read data |
					'-------------------------'
*/

str_t Request::url_decode(str_t &src)
{
	str_t ret;
	char c;
	size_t i, j;

	for (i = 0; i < src.length(); i++)
	{
		if (int(src[i]) == 37)
		{
			sscanf(src.substr(i + 1, 2).c_str(), "%lx", &j);
			c = static_cast<char>(j);
			ret += c;
			i += 2;
		}
		else
			ret += src[i];
	}
	return (ret);
}

int Request::parse_QueryString(size_t start)
{
	str_t line, query = _ressource.substr(start + 1, _ressource.size() - start);
	_query_string = query;
	_ressource = _ressource.substr(0, start);
	int i = 0;
	while ((line = newLine(query, "&")).size())
	{
		strPair p;
		line = url_decode(line);
		p.first = newLine(line, "=");
		p.second = line;
		_queryParam.insert(p);
		i++;
	}
	return (0);
}

size_t Request::parse_Url(str_t const &line)
{
	size_t pos = line.find('/', 0), end = pos;
	for (; !isspace(line[end]); end++)
		;

	_ressource = line.substr(pos, end - pos);

	size_t index = 0;
	while (true)
	{
		index = _ressource.find("//", index);
		if (index == std::string::npos)
			break;
		_ressource.replace(index, 2, "/");
		index += 1;
	}

	size_t q;
	if ((q = _ressource.find("?")) != _ressource.npos)
		parse_QueryString(q);
	for (; isspace(line[end]); end++);

	return (end);
}

int Request::parse_TopLine(str_t input)
{
	size_t found = 3;
	static str_t strTypes[3] = {"GET", "POST", "DELETE"};

	_flags |= PARSED_TOP;
	str_t line = newLine(input);
	for (size_t i = 0; i <= 2; i++)
	{
		if (line.find(strTypes[i]) != line.npos)
			found = i;
	}
	if (found == 3)
	{
		set_Error(400);
		return (1);
	}
	_type = int_pow(2, found);

	size_t pos = parse_Url(line);

	size_t end = pos;
	for (; line[end] && !isspace(line[end]); end++)
		;
	if (line.substr(pos, end) != SERVER_VERSION)
	{
		set_Error(505);
		return (2);
	}
	return (0);
}

int Request::add_Header(str_t line)
{
	strPair p;
	size_t limit = line.find(':');
	p.first = str_tolower(line.substr(0, limit++));
	while (isspace(line[limit++]));
	limit--;
	p.second = line.substr(limit, line.npos);
	_headers.insert(p);
	if (p.first == "content-length")
	{
		_cl = std::abs(atoi(p.second.c_str()));
		_flags |= PARSED_CL;
		if (_cl > _conf->client_max())
		{
			set_Error(413);
			return (1);
		}
	}
	else if (p.first == "content-type")
	{
		size_t pos = p.second.find("multipart/form-data");
		if (!pos)
		{
			_boundary = str_to_raw(p.second.substr(p.second.find("=", 20) + 1));
			_boundary.insert(_boundary.begin(), 2, '-');
			_flags |= PARSED_ISMULTI;
		}
	}
	else if (p.first == "host" && _conf->server_name().size())
	{
		str_v::iterator it;
		for (it = _conf->server_name().begin(); it != _conf->server_name().end(); it++)
		{
			str_t tmp = *it + ":" + to_string<int>(_port);
			if (p.second == tmp)
				break;
		}
		if (it == _conf->server_name().end())
		{
			set_Error(400);
			return (1);
		}
	}
	return (0);
}

int Request::add_Body(raw_str_t line, size_t plus)
{
	_body.push_back(line);
	_read_body += line.size() + plus;
	return (0);
}

void Request::set_Error(unsigned int code)
{
	_flags |= PARSED_ERROR;
	_error = code;
}


/*
					.---------.
					| Getters |
					'---------'
*/

int Request::fd()
{
	return (_fd);
}

unsigned int Request::type()
{
	return (_type);
}

unsigned int &Request::error()
{
	return (_error);
}

str_t Request::query_string()
{
	return (_query_string);
}

strMap &Request::headers()
{
	return (_headers);
}

std::vector<raw_str_t> &Request::body()
{
	return (_body);
}

unsigned int Request::read_body()
{
	return (_read_body);
}

unsigned int Request::cl()
{
	return (_cl);
}


/*
					.-------------.
					| Evaluations |
					'-------------'
*/

bool Request::isBoundary(raw_str_t line, bool &isEnd)
{
	if (line == _boundary)
		return (true);

	raw_str_t tmp =_boundary;
	tmp.push_back('-');
	tmp.push_back('-');

	if (raw_to_str(line).find(raw_to_str(tmp)) == 0)
	{
		isEnd = true;
		return (true);
	}
	return (false);
}

bool Request::done_Reading()
{
	return (_read_body == _cl);
}

bool Request::over_Read()
{
	return (_read_body > _cl);
}

bool Request::isBad()
{
	return (_flags & PARSED_ERROR);
}