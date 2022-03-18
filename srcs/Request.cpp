#include "Request.hpp"
#include "str_manips.hpp"
#include "find_nocase.hpp"
#include "flags.hpp"

Request::Request(int fd) : _fd(fd), _read_body(0)
{

}

// Request::Request(str_t input, int fd, size_t nl_head, size_t nl_body)
// : _fd((fd)), _nl_headers(nl_head), _nl_body(nl_body)
// {
// 	_flags = 0;
// 	std::cout << nl_head << " <- head  -  body -> " << nl_body <<std::endl;
// }

Request::Request(unsigned int error, int fd) : _fd(fd), _error(error)
{
	_flags = 0 | REQ_ISBAD;
}

// Request::Request(const Request &ref)
// : _fd(ref._fd), _type(ref._type), _headers(ref._headers),
// _ressource(ref._ressource), _queryParam(ref._queryParam)
// {}

Request	&Request::operator=(const Request &ref)
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
	}
	return (*this);
}

Request::~Request(void)
{
}

void	Request::reset()
{
	_type = 0;
	_headers.clear();
	_error = 0;
	_flags = 0;
	_nl_headers = 0;
	_nl_body = 0;
	_body.clear();
	_queryParam.clear();
	_read_body = 0;
	_query_string.clear();
}

str_t Request::url_decode(str_t &src)
{
	str_t ret;
	char c;
	size_t i, j;

	for (i = 0 ; i < src.length() ; i++)
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

int	Request::parse_QueryString(size_t start)
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

size_t	Request::parse_Url(str_t const &line)
{
	size_t pos = line.find('/', 0), end = pos;
	for (; !isspace(line[end]); end++);

	_ressource = line.substr(pos, end-pos);

	size_t q;
	if ((q  = _ressource.find("?")) != _ressource.npos)
		parse_QueryString(q);
	
	for (; isspace(line[end]); end++);
	return (end);
}

int	Request::parse_TopLine(str_t &input)
{
	size_t			found = R_DELETE + 1;
	static	str_t	strTypes[3] = {"GET", "POST", "DELETE"};

	str_t line = newLine(input);
	std::cout << "---------top line = " << line << std::endl;
	for (size_t i = R_GET; i <= R_DELETE; i++)
	{
		if (line.find(strTypes[i]) != line.npos)
			found = i;
	}
	if (found == R_DELETE + 1)
	{
		set_Error(400);
		return (1);
	}
	_type = found;

	size_t pos = parse_Url(line);

	size_t	end = pos;
	for (; line[end] && !isspace(line[end]); end++);
	if (line.substr(pos, end) != SERVER_VERSION)
	{
		set_Error(505);
		return (2);
	}
	_flags |= PARSED_TOP;
	return (0);
}

bool Request::isBad()
{
	return (_flags & PARSED_ERROR);
}

void	Request::set_Error(unsigned int code)
{
	_flags |= PARSED_ERROR;
	_error = code;
}

int	Request::fd()
{ return (_fd); }

unsigned int Request::type()
{ return (_type); }

unsigned int &Request::error()
{ return (_error); }

str_t Request::query_string()
{ return (_query_string); }

int Request::add_Header(str_t line)
{
	strPair p;
	size_t	limit = line.find(':');
	p.first = str_toUpper(line.substr(0, limit++));
	while (isspace(line[limit++]));
	limit--;
	p.second = line.substr(limit, line.npos);
	_headers.insert(p);
	//std::cout << p.first << std::endl;
	if (p.first == "content-length")
	{
		_cl = static_cast<size_t>(atoi(p.second.c_str()));
		_flags |= PARSED_CL; 
	}
	else if (p.first == "content-type")
	{
		size_t  pos = p.second.find("multipart/form-data");
		if (!pos)
		{
			_boundary = p.second.substr(p.second.find("=", 20) + 1);
			_flags |= PARSED_ISMULTI;
		}
	}
	return (0);
}

int Request::add_Body(str_t line)
{
	_body.push_back(line);
	_read_body += line.size() + 2;
	return (0);
}

bool	Request::isBoundary(str_t line)
{
	std::cout << line.find(_boundary) << std::endl;
	if (line.find(_boundary) != line.npos)			//this is not great. I would rather have find == 0 but right now it is not it.
		return (true);
	return (false);
}

bool Request::done_Reading()
{	
	std::cout << "are we done ? " << _read_body << "  " << _cl << std::endl;
	return (_read_body == _cl); }

bool Request::over_Read()
{	return (_read_body > _cl); }

// int	Request::parse(str_t input)
// {
// 	str_t				line;
// 	size_t					check = 0;
// 	int ret;
	
// 	 if (!input.size())
// 	 	return (1);
	
// 	if ((ret = parse_TopLine(input)))
// 	 	return (ret);
// 	_nl_headers--;
// 	while (check <= _nl_headers)
// 	{
// 		line = newLine(input);
// 		if (line  == "")
// 			break ;
// 		strPair p;
// 		size_t	limit = line.find(':');
// 		p.first = str_toUpper(line.substr(0, limit++));
// 		while (isspace(line[limit++]));
// 		limit--;
// 		p.second = line.substr(limit, line.npos);
// 		_headers.insert(p);
// 		check++;
// 	}
// 	check = 0;
// 	line = newLine(input);
// 	while (check < _nl_body)
// 	{
// 		line = newLine(input);
// 		_body.push_back(line);
// 		check++;
// 	}
// 	check = 0;
// 	// sum = _cl + _hl;

// 	std::cout << "-----HEADERS-----\n";
// 	for (strMap::iterator itt = _headers.begin(); itt != _headers.end(); itt++)
// 	{
// 		std::cout << itt->first << "  :  "  << itt->second << std::endl;
// 	}
// 	std::cout << "----------\n";

// 	std::cout << "-----BODY-----\n";
// 	for (std::vector<str_t>::iterator itb = _body.begin(); itb != _body.end(); itb++)
// 	{
// 		std::cout << *itb << std::endl;
// 	}
// 	std::cout << "----------\n";

// 	return (EXIT_SUCCESS);
// }

strMap	&Request::headers()
{ return (_headers); }

std::vector<str_t>	&Request::body()
{ return (_body); }