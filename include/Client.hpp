#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Response.hpp"


#define CL_PARSEDHEADERS 1
#define CL_PARSEDHEADLEN 2

class Client
{
	public:
		Client(int fd);
		//Client(const Client &ref);
		Client	&operator=(const Client &ref);
		~Client(void);
		void	add_request();
		int		add_data();
		void	respond();
		int		fd();
		char	*buff();

	private:
		Client(void);
		int						_fd;
		std::vector<Request>	_req;
		str_t					_input;
		char            		_buff[MAXREAD+1];
		unsigned int			_headers_len;
		unsigned int			_content_len;
		unsigned int			_flags;
};

#endif