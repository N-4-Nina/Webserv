#include "Request.hpp"
#include "str_manips.hpp"
#include "find_nocase.hpp"

Request::Request(str_t input, int fd, size_t nl_head, size_t nl_body)
: _fd((fd)), _nl_headers(nl_head), _nl_body(nl_body)
{
	parse(input);
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
	}
	return (*this);
}

Request::~Request(void)
{
}

int	Request::parse_QueryString(size_t start)
{
	str_t line, query = _ressource.substr(start + 1, _ressource.size() - start);
	_ressource = _ressource.substr(0, start);
	int i = 0;
	while ((line = newLine(query, "&")).size())
	{
		/*	PARAMS STILL NEED TO BE DECODED (FIND % AND CONVERT IN ASCII)
			THAT MAY OR MAY NOT BE THE PLACE TO DO IT 
			(MY GUESS IS IT IS)

			ALSO, SINCE WE USE A MAP, PARAMS WITH THE SAME NAME ARE OVERWRITTEN.
			THIS MIGHT OR MIGHT NOT BE A PROBLEM.
		*/
		strPair p;
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
		return (1);
	_type = found;

	size_t pos = parse_Url(line);

	size_t	end = pos;
	for (; line[end] && !isspace(line[end]); end++);
	if (line.substr(pos, end) != SERVER_VERSION)
		return (2);
	return (0);
}

int	Request::fd()
{ return (_fd); }

unsigned int Request::type()
{ return (_type); }

int	Request::parse(str_t input)
{
	str_t				line;
	size_t					check = 0;
	int ret;
	
	 if (!input.size())
	 	return (1);
	
	if ((ret = parse_TopLine(input)))
	 	return (ret);
	_nl_headers--;
	while (check <= _nl_headers)
	{
		line = newLine(input);
		if (line  == "")
			break ;
		strPair p;
		size_t	limit = line.find(':');
		p.first = str_toUpper(line.substr(0, limit++));
		while (isspace(line[limit++]));
		limit--;
		p.second = line.substr(limit, line.npos);
		_headers.insert(p);
		check++;
	}
	check = 0;
	line = newLine(input);
	while (check < _nl_body)
	{
		line = newLine(input);
		_body.push_back(line);
		check++;
	}
	check = 0;
	// sum = _cl + _hl;

	std::cout << "-----HEADERS-----\n";
	for (strMap::iterator itt = _headers.begin(); itt != _headers.end(); itt++)
	{
		std::cout << itt->first << "  :  "  << itt->second << std::endl;
	}
	std::cout << "----------\n";

	std::cout << "-----BODY-----\n";
	for (std::vector<str_t>::iterator itb = _body.begin(); itb != _body.end(); itb++)
	{
		std::cout << *itb << std::endl;
	}
	std::cout << "----------\n";

	return (EXIT_SUCCESS);
}

strMap	&Request::headers()
{ return (_headers); }

std::vector<str_t>	&Request::body()
{ return (_body); }