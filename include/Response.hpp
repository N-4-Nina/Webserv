#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "CGI.hpp"
#include "flags.hpp"
#include "Autoindex.hpp"


class Client;
class EvMa;
class Response
{
	public:
		Response(void);
		Response(Request &req, Config *conf, Client *client, EvMa *evma);
		Response(const Response &ref);
		Response	&operator=(const Response &ref);
		~Response(void);

		unsigned int	status();
		strMap			headers();
		FLAGS			flags();
		CGI				&cgi();

		void		prepare();
		void		prepare_cgi();
		int			send();
		void 		set_status(unsigned int s);
		void		set_body_cgi(Request req);
		void		check_cgi();
		void		kill_cgi();
		void		get_error_page();
		int			get_autoindex(Request req, str_t path, bool code);
		void		reset();

		static	strMap							_mimeTypes;
		static	std::map<unsigned int, str_t>	_codes;
		static	str_t							_error_page[2];
		size_t									_sent;

	private:
		void	upload_file(Request &req);
		void	delete_file(Request &req);
		void	select_location(Request &req);
		void	set_body_ress(Request &req, Config *conf);
		void	set_redir();
		void	add_mandatory_headers();
		void	set_headers(str_t path, Request & req);
		void	add_header(str_t key, str_t val);
		void	cgi_match(str_t uri, Request & req);
		str_t	add_head();

		CGI										_cgi;
		Client									*_client;
		Config									*_conf;
		FLAGS									_flags;
		Location								*_loc;
		unsigned int							_status;
		int										_fd;
		strMap									_headers;
		str_t									_route;
		str_t									_index;
		str_t									_head;
		str_t									_body;
		str_t									_res;
		str_t									_cgiret;		
};

#endif