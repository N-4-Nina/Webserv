#include "Client.hpp"
#include "find_nocase.hpp"
#include "str_manips.hpp"
#include <fstream>
#include "common.hpp"
#include "EvMa.hpp"

/*
					.--------------.
					| Constructors |
					'--------------'
*/

Client::Client(void): _serv(NULL), _fd(-1), _server_id(0),  _content_len(0), _ready(false)
{
}

Client::Client(int fd, Server *serv, EvMa *evma, int port) : _serv(serv), _fd(fd), _req(_fd), _content_len(0),  _ready(false)
{
	_evma = evma;
	_parse_flags = 0;
	_server_id = serv->id();
	memset(_buff, 0, MAXREAD+1);
	_port = port;
	_req._port = _port;
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
	_evma = ref._evma;
	_port = ref._port;
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
		_evma = ref._evma;
	}
	return (*this);
}

Client::~Client(void)
{
}


/*
					.---------.
					| Getters |
					'---------'
*/

Response	&Client::response()
{	return (_res);	}

int		Client::fd()
{ return (_fd); }

char	*Client::buff()
{ return (_buff); }

time_t	Client::expire()
{ return (_expire); }

bool	Client::isReady()
{ return (_ready); }

CGI		&Client::cgi()
{
	return (_res.cgi());
}


/*
					.--------.
					| Handle |
					'--------'
*/

void	Client::touch()
{
	_expire = time_in_ms() + TIMEOUT;
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


/*
						.------------.
						| Operations |
						'------------'
*/

/*		add_data is a super important function, it is the one where we perform
	the read operation on the socket. Of course, we can only read once per
	epoll call, so we need to keep track of the data we already read, so 
	it can safely work with any size of buffer.
		Though we do not read all at once -not to block the whole server in 
	case there is a lot of date-, we start to process them immediately line
	by line. This way if something is already wrong with the headers -for 
	example-, we can answer faster.
		The processing loop has three main case to handle: 
			- the first line of the request
			- the headers
			- the -optional- body
		This loop builds on the request object.
*/

int			Client::add_data()
{
	raw_str_t			input, line;
	raw_str_t::iterator pos;
	int					n;
	FLAGS 				&flags = _req._flags;

	memset(_buff, 0, MAXREAD + 1);
	/*
		Reading on the socket. please note we check both 0 and < 0, but because
		the socket is non-blocking, we don't actually do anything if we get -1;
		it is because errno could be EAGAIN or EWOULDBLOCK, which is not per se
		an error. So we just wait to see if more data come.
	*/
	n  = read(_fd, _buff, MAXREAD);
	if (n == 0)
		return (0);
	else if (n < 0)
		return (-1);
	this->touch();
	log(_serv, this, "Read " + to_string(n) + " octets.");

	input = char_to_raw(_buff, n);

	/* Handling remaining data. */
	if (_remain.size())
	{
		_read_pos = _remain.size();
		input.insert( input.begin(), _remain.begin(), _remain.end() );
		_remain.clear();
	}

	/* Processing loop */
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
			if (_req.type() == POST && !(flags & PARSED_CL))
			{
				_req.set_Error(411);
				_ready = true;
				break;
			}
			else if (_req.type() != POST)
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

/*		Respond is also a crucial function:
			-It builds the response (response constructor).
			-It determine if we are ready to actually write it on the socket -check_cgi-.
			-It check if we're not timing out while building the response.
			-It prepares the request for writing (setting headers)
			-Finally, it writes on the socket chunk by chunk (like reading). 
*/
int	Client::respond(str_t &reason)
{
	if (!(_res.flags() & RES_STARTED))
	{
		log(_serv, this, "Responding");
		_res = Response(_req, _serv->conf(), this, _evma);
		this->touch();
	}
	else
		_res.check_cgi();
	
	if (_expire < time_in_ms())
	{
		reason = "timeout.";
		if (!_res._sent)
		{
			if (( _res.flags() & RES_ISCGI))
			{
				_res.set_status(504);		//gateway timeout
				_res.kill_cgi();
			}
			else
				_res.set_status(408);		//request timeout
			_res.prepare();
		}
		int ret = _res.send();
		if (!ret)
		{
			this->reset();
			return (1);
		}
		else if (ret < 0)
		{
			reason = "write error";
			return (ret);
		}
	}
	else if ((_res.flags() & RES_READY))
	{
		if ((_res.flags() & RES_ISCGI) && _res.status() != 502)
			_res.prepare_cgi();
		else
			_res.prepare();
		this->touch();
		int ret = _res.send();
		if (!ret)
		{
			ret = _res.flags() & RES_CLOSE;
			reason = "specified by peer.";
			this->reset();
			return (ret);
		}
		else if (ret < 0)
		{
			reason = "write error";
			return (ret);
		}
		return (0);
	}
	return (0);
}