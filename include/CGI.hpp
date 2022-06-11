#ifndef CGI_HPP
# define CGI_HPP

# include "common.hpp"
# include "Request.hpp"
# include "str_manips.hpp"

class EvMa;

class CGI
{
	public:
		CGI(EvMa *evma = NULL);
		CGI(const CGI &ref);
		CGI	&operator=(const CGI &ref);
		~CGI();

		EvMa	*_evma;
		void	set_binary(str_t path);
		void	set_script_name(str_t script_name);
		str_t 	body();
		str_t	binary();
		str_t	script_name();
		int		pid();
		void	exec_cgi(str_t target, Request req);
		void	check(FLAGS *flags, unsigned int *code);
		void	reset();
		void	close_fd();

	private:
		str_t	_binary;
		str_t	_body;
		str_t	_script_name;
		int		_pid;
		int		_status;
		int		_fd_io[2];
		bool	_done;
		char	**build_cgi_env(Request req, str_t target);
		void	get_host_port(Request req, strMap &envMap);
		void	free_str_tab(char **str_tab);
		void	free_cgi(char **args, char **env);
};

void DEBUG_display_cgi_env(char **env, char **args);

#endif