#include "Client.hpp"
#include "find_nocase.hpp"
#include <fstream>

Client::Client(void): _fd(-1), _server_id(0), _serv(NULL),  _content_len(0), _ready(false)
{
}

Client::Client(int fd, Server *serv) : _fd(fd), _serv(serv), _req(_fd), _content_len(0),  _ready(false)
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
		//_req.clear();
		_req = ref._req;
		//_input = ref._input;
		strcpy(_buff, ref._buff);
		_content_len = ref._content_len;
		_serv = ref._serv;
	}
	return (*this);
}

Client::~Client(void)
{
}

// void	Client::add_request()
// {
// 	_req = Request(_input, _fd, _nl_headers, _nl_body);

// 	std::cout << "\nparsed a request\n" << std::endl;
// 	//std::cout << "remaining : " << _input << std::endl;
// 	//_input.clear();
// 	//_input = _input.substr(_headers_len + _content_len);
// 	//std::cout << "remaining : " << _input << std::endl;
// 	//shutdown(_fd, 0);
// 	_headers_len = 0;
// 	_content_len = 0;
// }

// void	Client::add_request(unsigned int error)
// {
// 	_req = Request(error, _fd);

// 	std::cout << "\nparsed an error request\n" << std::endl;
// 	//std::cout << "remaining : " << _input << std::endl;
// 	//_input.clear();
// 	//_input = _input.substr(_headers_len + _content_len);
// 	//std::cout << "remaining : " << _input << std::endl;
// 	//shutdown(_fd, 0);
// 	_headers_len = 0;
// 	_content_len = 0;
// }

int		Client::fd()
{ return (_fd); }

char	*Client::buff()
{ return (_buff); }

time_t	Client::expire()
{ return (_expire); }

bool	Client::isReady()
{ return (_ready); }


int			Client::add_data()
{
	str_t			input, line;
	int n;
	FLAGS &flags = _req._flags;

	memset(_buff, 0, MAXREAD + 1);
	
	n  = read(_fd, _buff, MAXREAD);
	input = std::string(_buff);

	
	while (input != "")
	{
		if (_remain != "")
		{
			input = _remain + input;
			_remain = "";
		}
		if (n == MAXREAD && input.find(CRLF) == input.npos)
		{
			_remain = input;
			return (0);
		}	
		line = newLine(input);
		if ( !(flags & PARSED_TOP))
			_req.parse_TopLine(line);
		else if (!(flags & PARSED_HEADERS) && line == "")
		{
			flags |= PARSED_HEADERS;
			if (_req.type() == R_POST && !(flags & PARSED_CL))
			{
				_req.set_Error(411);
				_ready = true;
				break;
			}
			else if (_req.type() != R_POST)
			{
				_ready = true;
				input.clear();
				_remain = "";
				_ready = true;
				break;
			}
		}
		else if (! (flags & PARSED_HEADERS))
			_req.add_Header(line);
		//else if ((flags & PARSED_ISMULTI) && _req.isBoundary(line))
		//	continue;
		else if ((flags & PARSED_CL))
		{
			_req.add_Body(line);
			std::cout << line << "\n";
			if (_req.done_Reading())
			{
				_ready = true;
				input.clear();
				_remain = "";
			}
			//else if (_req.over_Read())
			//{
			//	_req.set_Error(413);
			//	_ready = true;
			//	_remain = "";
			//	break;
			//}
		}
	}
	return (0);
}

// int		Client::add_data()
// {
// 	//std::cout << "\n\n ADD_DATA (fd = " << _fd <<  ")\n";
// 	size_t pos;
// 	memset(_buff, 0, MAXREAD);
// 	int n;

// 	n  = read(_fd, _buff, MAXREAD);
// 	_input = _input + str_t(_buff);	
// 	if (n == 0)
// 	{
// 		_ready = true;
// 		//close(_fd);
// 		return (1);
// 	}
// 	//std::cout << _input;
// 	if (!(_parse_flags & PARSED_CL) && (pos = find_nocase<std::string>(_input, "CONTENT-LENGTH")) != _input.npos)
// 	{
// 		if (_input.find("\n", pos) != _input.npos)
// 		{
// 			std::cout << "found content len" << std::endl;
// 			_content_len = atoi(_input.substr(pos + 16).c_str());
// 			_parse_flags |= PARSED_CL;
// 		}
// 	}
// 	size_t i;
// 	if (((pos = _input.find("\r\n\r\n")) != _input.npos) || ((pos = _input.find("\n\n")) != _input.npos))
// 	{
// 		if (!(_parse_flags & PARSED_CNL))
// 		{
// 			for (i = 0; i < pos; i++)
// 				if (_input[i] == '\n')
// 					_nl_headers++;
// 			_parse_flags |= PARSED_CNL;
// 		}
// 		pos += (_input[pos] == '\r') ? 4 : 2;
// 		i = 0;
// 		if (!(_parse_flags & PARSED_BNL))
// 		{
// 			std::cout << "\n-\n";
// 			for (; _input[i + pos] && i < _content_len; i++)
// 			{
// 				// if (!_input[i + pos])
// 				// {
// 				// 	_nl_body = 0;
// 				// 	_parse_flags = _parse_flags & ~PARSED_BNL;
// 				// 	break;
// 				// }
// 				std::cout << _input[i + pos];
// 				if (_input[i + pos] == '\n')
// 					_nl_body++;
// 				_parse_flags |= PARSED_BNL;
// 			}
// 			std::cout << "\n-\n";
// 		}
// 		//std::cout << _input.substr(pos, i);
// 		std::cout << "i = " << i << " _content length = " << _content_len - _nl_body << "nlbody = "  << _nl_body << std::endl;
// 		if (i == _content_len -  _nl_body)
// 		{
// 			std::ofstream stream;
// 			stream.open("./request");
// 			stream << _input;
// 			stream.close();
// 			add_request();
// 			_input.clear();
// 			_ready = true;
// 			//shutdown(_fd, SHUT_RD);
// 		}
// 		else
// 		{
// 			if (!_input[i + pos])
// 			{
// 				_nl_body = 0;
// 				_parse_flags = _parse_flags & ~PARSED_BNL;
// 			}
// 		}
		
// 	}

// 	return (0);
// }

void	Client::respond()
{
	std::cout << "responding\n";
	Response res(_req, _serv->conf());
	res.send();
	_ready = false;
	_req.reset();
	//_input.clear();
	memset(_buff, 0, MAXREAD+1);
	_parse_flags = 0;
	_content_len = 0;
	close (_fd);

}

void	Client::touch()
{
	_expire = time_in_ms() + 5000;
}
