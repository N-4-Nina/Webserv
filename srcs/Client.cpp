#include "Client.hpp"
#include "find_nocase.hpp"

Client::Client(void): _fd(-1), _server_id(0), _serv(NULL), _nl_headers(0), _nl_body(0), _content_len(0), _ready(false)
{
}

Client::Client(int fd, Server *serv) : _fd(fd), _serv(serv), _nl_headers(0), _nl_body(0), _content_len(0), _ready(false)
{
	_parse_flags = 0;
	_server_id = serv->id();
	memset(_buff, 0, MAXREAD+1);
	(void)_server_id;
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
		_serv = ref._serv;
	}
	return (*this);
}

Client::~Client(void)
{
}

void	Client::add_request()
{
	_req.push_back(Request(_input, _fd, _nl_headers, _nl_body));

	std::cout << "\nparsed a request\n" << std::endl;
	//std::cout << "remaining : " << _input << std::endl;
	//_input.clear();
	//_input = _input.substr(_headers_len + _content_len);
	//std::cout << "remaining : " << _input << std::endl;
	shutdown(_fd, 0);
	_headers_len = 0;
	_content_len = 0;
}

int		Client::fd()
{ return (_fd); }

char	*Client::buff()
{ return (_buff); }

time_t	Client::expire()
{ return (_expire); }

bool	Client::isReady()
{ return (_ready); }

int		Client::add_data()
{
	//std::cout << "\n\n ADD_DATA (fd = " << _fd <<  ")\n";
	size_t pos;
	memset(_buff, 0, MAXREAD);
	int n;

	n  = read(_fd, _buff, MAXREAD);
	_input = _input + str_t(_buff);	
	if (n == 0)
	{
		_ready = true;
		//close(_fd);
		return (1);
	}
	//std::cout << _input;
	if (!(_parse_flags & PARSED_CL) && (pos = find_nocase<std::string>(_input, "CONTENT-LENGTH")) != _input.npos)
	{
		std::cout << "found content len" << std::endl;
		if (_input.find("\n", pos) != _input.npos)
		{
			_content_len = atoi(_input.substr(pos + 16).c_str());
			_parse_flags |= PARSED_CL;
		}
	}
	size_t i;
	if (((pos = _input.find("\r\n\r\n")) != _input.npos))
	{
		if (!(_parse_flags & PARSED_CNL))
		{
			for (i = 0; i < pos; i++)
				if (_input[i] == '\n')
					_nl_headers++;
			_parse_flags |= PARSED_CNL;
		}
		i = 0;
		if (!(_parse_flags & PARSED_BNL))
		{
			for (; i < _content_len; i++)
			{
				if (!_input[i + pos + 4])
				{
					_nl_body = 0;
					_parse_flags = _parse_flags & ~PARSED_BNL;
					break;
				}
				if (_input[i + pos + 4] == '\n')
					_nl_body++;
				_parse_flags |= PARSED_BNL;
			}
		}
		if (i == _content_len)
		{
			add_request();
			_input.clear();
			_ready = true;
			//shutdown(_fd, SHUT_RD);
			
		}
		
	}

	return (0);
		
}

void	Client::respond()
{
	std::cout << "responding\n";
	for (size_t i = 0; i < _req.size(); i++)
	{
		std::cout << "response " << i << std::endl;
		Response res(_req[i], _serv->conf());
		res.send();
	}
	_ready = false;
	close (_fd);
}

void	Client::touch()
{
	_expire = time_in_ms() + 5000;
}
