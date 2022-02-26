#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "Location.hpp"

enum 
{
	/* 	FOR NOW THIS ENUM IGNORES WEBDAV STATUS WHICH ARE DEFINED
		IN RFC 2518;  AND RFC 2616
	*/
	CONTINUE = 100,
	SWITCH_PROTOCOL = 101,
	//PROCESSING = 102,		//RFC 2518
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	TOO_MANY_REDIRECTS = 310
//TO BE CONTINUED, SEE https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
// OR WIKIPEDIA
};

class Response
{
	public:
		Response(Request &req, Config *conf);
		//Response(const Response &ref);
		//Response	&operator=(const Response &ref);
		~Response(void);
		void 			set_status(unsigned int s);
		unsigned int	status();
		void			send();
	private:
		void	set_route(Request &req);
		Response(void);
		Config									*_conf;
		std::string								_route;
		unsigned int							_status;
		int										_fd;
		static	std::map<unsigned int, str_t>	_messages;
		void									*_body;
};

#endif