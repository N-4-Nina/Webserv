#include "Client.hpp"
#include "find_nocase.hpp"
#include "str_manips.hpp"
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
	_req._conf = serv->conf();
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
		_req._conf = ref._req._conf;
	}
	return (*this);
}

Client::~Client(void)
{
}

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
	raw_str_t			input, line;
	raw_str_t::iterator pos;
	int n;
	FLAGS &flags = _req._flags;

	memset(_buff, 0, MAXREAD + 1);
	
	n  = read(_fd, _buff, MAXREAD);
	input = char_to_raw(_buff, n);

	if (_remain.size())
	{
		_read_pos = _remain.size();
		input.insert( input.begin(), _remain.begin(), _remain.end() );
		_remain.clear();
	}
	while (input.size())
	{
		pos = raw_find(input, CRLF, 2, _read_pos);
		if (pos == input.end())
		{
			if ((flags & PARSED_CL) && _req.cl() == _req.read_body() + input.size())
			{
				_req.add_Body(line);
				if (_req.done_Reading())
				{
					_ready = true;
					input.clear();
					_remain.clear();
				}
			}
			else
			{
				_remain = input;
				input.clear();
				_read_pos = _remain.size();
				return (0);
			}
		}
		_read_pos = 0;
		line = raw_newLine(input, pos);
		if ( !(flags & PARSED_TOP))
			{ if (_req.parse_TopLine(raw_to_str(line)))
				_ready = true; }
		else if (!(flags & PARSED_HEADERS) && line.size() == 0)
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
				_remain.clear();
				_ready = true;
				break;
			}
		}
		else if (! (flags & PARSED_HEADERS))
		{
			if	(_req.add_Header(raw_to_str(line)))
			{
				_ready = true;
				fsync(_fd);
				return(0);
			}
		}
		else if ((flags & PARSED_CL))
		{
			_req.add_Body(line);
			if (_req.done_Reading())
			{
				_ready = true;
				input.clear();
				_remain.clear();
			}
			else if (_req.over_Read())
			{
				_req.set_Error(413);
				_ready = true;
				_remain.clear();
				break;
			}
		}
	}
	return (0);
}


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
