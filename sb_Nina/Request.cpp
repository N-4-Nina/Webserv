#include "../include/Request.hpp"
#include "../include/str_manips.hpp"

Request::Request(void)
{
}

Request::Request(const Request &ref)
{
}

Request	&Request::operator=(const Request &ref)
{
	return (*this);
}

Request::~Request(void)
{
}

int	Request::parse_TopLine(str_t topLine)
{
	size_t			found = R_DELETE + 1;
	static	str_t	strTypes[3] = {"GET", "POST", "DELETE"};

	for (size_t i = R_GET; i <= R_DELETE; i++)
	{
		if (topLine.find(strTypes[i]) != topLine.npos)
			found = i;
	}
	if (found == R_DELETE + 1)
		return (1);
	_type = found;

	size_t	pos = strTypes[found].length();
	for (; isspace(topLine[pos]); pos++);
	for (; !isspace(topLine[pos]); pos++);
	_ressource = topLine.substr(strTypes[found].length(), pos);

	for (; isspace(topLine[pos]); pos++);
	size_t	versionPos = pos;
	for (; !isspace(topLine[versionPos]); versionPos++);
	if (topLine.substr(pos, versionPos) != "HTTP/1.1")
		return (2);
	return (0);
}

str_t	Request::newLine(str_t &in)
{
	size_t	pos = in.find("CRLF");
	str_t	out = in.substr(0, pos);
	in = in.substr(pos + 2, in.npos);
	return (out);
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
		p.second = line.substr(limit, line.npos);
		_headers.insert(p);
	}
	return (EXIT_SUCCESS);
}