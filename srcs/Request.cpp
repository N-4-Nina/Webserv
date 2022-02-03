#include "../include/Request.hpp"
#include "../include/str_manips.hpp"

Request::Request(str_t input, int fd) : _fd((fd))
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

int	Request::parse_TopLine(str_t line)
{
	size_t			found = R_DELETE + 1;
	static	str_t	strTypes[3] = {"GET", "POST", "DELETE"};

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



int	Request::parse(str_t input)
{
	int ret;
	str_t line;

	if ((ret = parse_TopLine(newLine(input))))
		return (ret);
	
	while ((line = newLine(input)).size())
	{
		strPair p;
		size_t	limit = line.find(':');
		p.first = str_toUpper(line.substr(0, limit++));
		while (isspace(line[limit++]));
		limit--;
		p.second = line.substr(limit, line.npos);
		_headers.insert(p);
	}
	return (EXIT_SUCCESS);
}

strMap	&Request::headers()
{ return (_headers); }