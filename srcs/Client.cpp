#include "Client.hpp"
#include "find_nocase.hpp"

Client::Client(void)
{
}

Client::Client(int fd) : _fd(fd)
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
	}
	return (*this);
}

Client::~Client(void)
{
}

void	Client::add_request()
{
	_req.push_back(Request(_input.substr(0, _headers_len + _content_len), _fd));
	//std::cout << "parsed request :\n" << _input.substr(0, _headers_len + _content_len) << std::endl;
	//std::cout << "remaining : " << _input << std::endl;
	_input = _input.substr(_headers_len + _content_len);
	//std::cout << "remaining : " << _input << std::endl;
	_headers_len = 0;
	_content_len = 0;
}

int		Client::fd()
{ return (_fd); }

char	*Client::buff()
{ return (_buff); }

time_t	Client::expire()
{ return (_expire); }

int		Client::add_data()
{
	//std::cout << "\n\n ADD_DATA (fd = " << _fd <<  ")\n";
	size_t pos;
	memset(_buff, 0, MAXREAD);
	int n;

	n = recv(_fd, _buff, MAXREAD - 1, 0);
	//n  = read(_fd, _buff, MAXREAD-1);
	_input = _input + str_t(_buff);
	memset(_buff, 0, MAXREAD);
	
	
	if ((pos = find_nocase<std::string>(_input, "CONTENT-LENGTH")) != _input.npos)
	{
		if (find_nocase<std::string>(_input, "\n", pos) != _input.npos)
		{
			_content_len = atoi(_input.substr(pos + 17).c_str());
		}
	}
	if (((pos = _input.find("\r\n\r\n")) != _input.npos))
	{
		_headers_len  = pos + 4;
	}

	if (_input.size() >= _headers_len + _content_len && _input.size())
	{
        add_request();
		_input = _input.substr(_headers_len + _content_len);
		//std::cout << "n is :" << n << std::endl;
	}

	char            buff[MAXREAD+1];
	snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 \r\n\r\n<!OKDOCTYPE html>\n<head>\n</head>\n<body>\n<div>Hello There :)</div>\n<img src=\"image.jpg\"/>\n</body>\n</html>");

    write(_fd, buff, strlen(buff));
   	close(_fd);

	if (n < 0)
	{
		std::cout << _input << std::endl;
		return (1);
	}
	return (0);
		
}

void	Client::respond()
{
	std::cout << "responding\n";
	for (size_t i = 0; i < _req.size(); i++)
	{
		std::cout << "response " << i << std::endl;
		Response res(_req[i]);
		res.send();
	}
	close (_fd);
}

void	Client::touch()
{
	_expire = time_in_ms() + 5000;
}