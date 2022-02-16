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
{
	return (_fd);
}

char	*Client::buff()
{
	return (_buff);
}

int		Client::add_data()
{
	std::cout << "\n\n ADD_DATA (fd = " << _fd <<  ")\n";
	size_t pos;
	memset(_buff, 0, MAXREAD);
	int n;
	n  = read(_fd, _buff, MAXREAD-1);
	if (n < 0)
		return (1);
	std::cout << n << std::endl;
	_input = _input + str_t(_buff);
	std::cout << _input << std::endl;
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
		std::cout << "n is :" << n << std::endl;
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