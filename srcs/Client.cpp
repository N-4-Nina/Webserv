#include "Client.hpp"
#include "find_nocase.hpp"
#include "str_manips.hpp"
#include <fstream>
#include "common.hpp"

Client::Client(void): _serv(NULL), _fd(-1), _server_id(0),  _content_len(0), _ready(false)
{
}

Client::Client(int fd, Server *serv) :  _serv(serv), _fd(fd), _req(_fd), _content_len(0),  _ready(false)
{
	_parse_flags = 0;
	_server_id = serv->id();
	memset(_buff, 0, MAXREAD+1);
	_req._conf = serv->conf();
	_read_pos = 0;
}

Client::Client(const Client &ref)
{
	_fd = ref._fd;
	_server_id = ref._server_id;
	_serv = ref._serv;
	_req = ref._req;
	_remain = ref._remain;
	strcpy(_buff, ref._buff);
	_parse_flags = ref._parse_flags;
	_read_pos = ref._read_pos;
	_content_len = ref._content_len;
	_expire = ref._expire;
	_ready = ref._ready;
	_req._conf = ref._req._conf;
	_res = ref._res;
}

Client	&Client::operator=(const Client &ref)
{
	if (&ref != this)
	{
		_fd = ref._fd;
		_server_id = ref._server_id;
		_serv = ref._serv;
		_req = ref._req;
		_remain = ref._remain;
		strcpy(_buff, ref._buff);
		_parse_flags = ref._parse_flags;
		_read_pos = ref._read_pos;
		_content_len = ref._content_len;
		_expire = ref._expire;
		_ready = ref._ready;
		_req._conf = ref._req._conf;
		_res = ref._res;
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
	
	//if (n == -1)
	//	return (-1);
	if (n <= 0)					//also this....
		return (1);
	log(_serv, this, "Read " + to_string(n) + " octets.");
	input = char_to_raw(_buff, n);
	if (_remain.size())
	{
		_read_pos = _remain.size();
		input.insert( input.begin(), _remain.begin(), _remain.end() );
		_remain.clear();
	}

	/* this next block up to line 105 is very sus... do we even need it ?..*/
	//pos = raw_find(input, CRLF, 2, _read_pos);
	// if (pos == input.end())
	// {
	// 	if ((flags & PARSED_CL) && _req.cl() == _req.read_body() + input.size())
	// 	{
	// 		_req.add_Body(line);
	// 		if (_req.done_Reading())
	// 		{
	// 			_ready = true;
	// 			input.clear();
	// 			_remain.clear();
	// 		}
	// 	}
	// 	else if (!(flags & PARSED_HEADERS))
	// 	{
	// 		_remain = input;
	// 		input.clear();
	// 		_read_pos = _remain.size();
	// 		return (0);
	// 	}
	// }

	while (input.size())
	{
		pos = raw_find(input, CRLF, 2, _read_pos);
		_read_pos = 0;
		line = raw_newLine(input, pos);
		
		if ( !(flags & PARSED_TOP))
		{
			if (_req.parse_TopLine(raw_to_str(line)))
			{
				_ready = true;
				break;
			}
		}
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
				input.clear();
				_remain.clear();
				_ready = true;
				break;
			}
		}
		else if (!(flags & PARSED_HEADERS))
		{
			if	(_req.add_Header(raw_to_str(line)))
			{
				_ready = true;
				fsync(_fd);
				return (0);
			}
		}
		else if ((flags & PARSED_CL))
		{
			if (!line.size() && raw_find(input, CRLF, 2) == input.end())
				_req.add_Body(input, 0);
			else
				_req.add_Body(line, 2);
			log(_serv, this, "Read " + to_string(_req.read_body()) + " oct / " + to_string(_req.cl()) + " oct");
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


int	Client::respond()
{
	if (!(_res.flags() & RES_STARTED))
	{
		log(_serv, this, "Responding");
		_res = Response(_req, _serv->conf(), this);
	}
	else
		_res.check_cgi();
	
	if ((_res.flags() & RES_READY))
	{
		_res.send();
		this->reset();
		return (0);
	}
	return (1);
}

void	Client::reset()
{
	_ready = false;
	_req.reset();
	_res.reset();
	_remain.clear();
	memset(_buff, 0, MAXREAD+1);
	_parse_flags = 0;
	_content_len = 0;
	_parse_flags = 0;
	_read_pos = 0;
	_content_len = 0;
}

void	Client::touch()
{
	_expire = time_in_ms() + TIMEOUT;
}
