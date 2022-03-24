#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "CGI.hpp"
#include "flags.hpp"

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
	TOO_MANY_REDIRECTS = 310,
	INETERNAL_SERVER_ERROR = 500
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
		static	strMap							_mimeTypes;
		static	std::map<unsigned int, str_t>	_codes;
		static	str_t							_error_page[2];
		str_t									exceCGI(Request req);
	private:
		void	upload_file(Request &req);
		void	select_location(Request &req);
		void	set_body_ress(Request &req, Config *conf);
		void	set_redir();
		void	set_headers(str_t path);
		void	add_header(str_t key, str_t val);
		void	get_error_page();
		//bool	find_ressource();
		bool	cgi_match(str_t uri);
		str_t	add_head();
		strMap	headers();
		Response(void);
		Config									*_conf;
		str_t								_route;
		FLAGS									_flags;
		Location								*_loc;
		unsigned int							_status;
		int										_fd;
		strMap									_headers;
		
		str_t									_index;
		str_t									_head;
		str_t									_body;
};

#endif