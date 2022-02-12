#include "Client.hpp"
#include "find_nocase.hpp"

Client::Client(void)
{
}

Client::Client(int fd) : _fd(fd), _flags(0)
{
	memset(_buff, 0, MAXREAD+1);
}

// Client::Client(const Client &ref)
// {
// }

Client	&Client::operator=(const Client &ref)
{
	if (&ref != this)
	{
		_fd = ref._fd;
		_req.clear();
		_req = ref._req;
		//_input = ref._input;
		strcpy(_buff, ref._buff);
		_headers_len = ref._headers_len;
		_content_len = ref._content_len;
		_flags = ref._flags;
	}
	return (*this);
}

Client::~Client(void)
{
}

void	Client::add_request()
{
	_req.push_back(Request(_input, _fd));
}

int		Client::fd()
{
	return (_fd);
}

char	*Client::buff()
{
	return (_buff);
}

int		Client::add_data()
{
	size_t pos;
	int n = read(_fd, _buff, MAXREAD-1);
	std::cout << n << std::endl;
	_input = _input + str_t(_buff);

	if (!(_flags & CL_PARSEDHEADLEN) && (pos = find_nocase<std::string>(_input, "CONTENT-LENGTH")) != _input.npos)
	{
		if (find_nocase<std::string>(_input, "\n", pos) != _input.npos)
		{
			_content_len = atoi(_input.substr(pos + 17).c_str());
			_flags = _flags | CL_PARSEDHEADLEN;
		}
	}
	if (!(_flags & CL_PARSEDHEADERS) && ((pos = _input.find("\r\n\r\n")) != _input.npos))
	{
		_headers_len  = pos + 4;
		_flags = _flags | CL_PARSEDHEADERS;
	}

	if (_input.size() == _headers_len + _content_len)
        add_request();
	return (n);
}

void	Client::respond()
{
	for (size_t i = 0; i < _req.size(); i++)
	{
		Response res(_req[i]);
		res.send();
	}
	close (_fd);
}